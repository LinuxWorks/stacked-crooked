#include <boost/container/flat_set.hpp>
#include <atomic>
#include <unordered_map>
#include <map>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <deque>
#include <thread>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>


struct Packet
{
    explicit Packet(int flowId) : mFlowId(flowId) {}
    std::size_t size() const { return mData.size(); }
    std::string mData;
    int mFlowId = 0;
};

using Duration  = std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;


Timestamp gStartTime = Clock::now();



struct BBInterface;



struct Flow
{
    Flow() :
        mPacket([]{ static int flowId; return flowId++; }())
    {
    }

    void set_mbps(int64_t mbps)
    {
        mBytesPerSecond = 1e6 * mbps / 8;
    }

    void consume(std::deque<Packet*>& packets, Timestamp current_time)
    {
        if (mNextTransmission == Timestamp())
        {
            mNextTransmission = current_time;
        }

        for (auto i = 0; i != 2; ++i)
        {
            if (current_time < mNextTransmission)
            {
                break;
            }
            packets.push_back(&mPacket);
            mTxBytes += mPacket.size();
            auto next = std::chrono::nanoseconds(long(1e9 * mPacket.size() / mBytesPerSecond));
            mNextTransmission += next;
        }
    }
    
    Packet mPacket;
    Timestamp mNextTransmission = Timestamp();
    double mBytesPerSecond = 1e9 / 8;
    int64_t mTxBytes = 0;
};


struct BBInterface
{
    void set_rate_limit(double bytes_per_ns)
    {
        mBytesPerNs = bytes_per_ns;
    }

    void add_flow(Flow& flow)
    {
        mFlows.insert(&flow);
    }

    void remove_flow(Flow& flow)
    {
        mFlows.erase(&flow);
    }

    void consume(std::deque<Packet*>& packets, Timestamp current_time)
    {
        if (mPackets.empty())
        {
            for (Flow* flow : mFlows)
            {
                flow->consume(mPackets, current_time);
            }
            if (mPackets.empty())
            {
                return;
            }
        }

        Packet* front_packet = mPackets.front();
        auto packet_size = front_packet->size();

        if (mBytesPerNs && mBucket < packet_size)
        {
            if (mLastTime == Timestamp())
            {
                mLastTime = current_time;
            }
            std::chrono::nanoseconds elapsed_ns = current_time - mLastTime;

            auto bucket_increment = elapsed_ns.count() * mBytesPerNs / 1e9;
            auto new_bucket = std::min(mBucket + bucket_increment, mMaxBucketSize);
            if (new_bucket < packet_size)
            {
                return;
            }

            mBucket = new_bucket;
            mLastTime = current_time;
        }


        packets.push_back(front_packet);
        mTxBytes += packet_size;
        mPackets.pop_front();

        if (mBytesPerNs)
        {
            mBucket -= packet_size;
        }
    }

    double mBytesPerNs = 2 * 1e9 / 8;
    double mMaxBucketSize = 8 * 1024;
    double mBucket = mMaxBucketSize;
    Timestamp mLastTime = Timestamp();
    boost::container::flat_set<Flow*> mFlows;
    std::deque<Packet*> mPackets;
    int64_t mTxBytes = 0;
};


struct Socket
{
    void send_batch(Timestamp ts, const std::deque<Packet*>& packets)
    {
        auto total_size = 0ul;
        for (auto p : packets)
        {
            auto size = p->size();
            total_size += size;
            mSizes[size]++;
        }

        mTxBytes += total_size;

        if (mTimestamp == Timestamp())
        {
            mTimestamp = Clock::now();
        }


        std::chrono::nanoseconds elapsed_ns = ts - mTimestamp;

        if (elapsed_ns > std::chrono::seconds(1))
        {
            std::cout << "elapsed_ns=" << elapsed_ns.count() << " TxBytes=" << mTxBytes << " ByteRate=" << int(10 * 8000 * mTxBytes / elapsed_ns.count())/10.0 << "Mbps" << std::endl;
            mTxBytes = 0;
            mTimestamp = ts;
            for (auto& el : mSizes)
            {
                std::cout << ' ' << el.first << ": " << el.second << std::endl;
            }
        }

    }

    uint64_t mTxBytes = 0;
    Timestamp mTimestamp = Timestamp();
    std::map<int, int> mSizes;
};


struct PhysicalInterface
{
    PhysicalInterface(std::size_t num_interfaces) :
        mBBInterfaces(num_interfaces),
        mThread(&PhysicalInterface::run_thread, this)
    {
    }
    
    PhysicalInterface(const PhysicalInterface&) = delete;
    PhysicalInterface& operator=(const PhysicalInterface&) = delete;
    
    ~PhysicalInterface()
    {
        stop();
    }

    void stop()
    {
        if (!mQuit.exchange(true))
        {
            mThread.join();
        }
    }

    std::vector<BBInterface>& getBBInterfaces()
    {
        return mBBInterfaces;
    }

private:
    void run_thread()
    {
        std::deque<Packet*> packets;
        
        while (!mQuit)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            auto now = Clock::now();

            for (BBInterface& bbinterface : mBBInterfaces)
            {
                bbinterface.consume(packets, now);
            }

            if (!packets.empty())
            {
                mSocket.send_batch(now, packets);
                packets.clear();
            }
        }
    }
    
    std::atomic<bool> mQuit{false};
    std::vector<BBInterface> mBBInterfaces;
    Socket mSocket;
    std::mutex mMutex;
    std::thread mThread;
};


int main()
{
    PhysicalInterface physicalInterface(48);
    BBInterface* bbInterfaces = physicalInterface.getBBInterfaces().data();

    enum { num_interfaces = 2 };

    physicalInterface.getBBInterfaces().resize(num_interfaces);
    enum { num_flows = 4 };
    Flow flows[num_interfaces][num_flows];

    int sizes[num_flows] = { 64, 128, 512, 1024 };
    int mbps[num_flows] = { 250, 250, 250, 250 };


    for (auto interface_id = 0; interface_id != num_interfaces; ++interface_id)
    {
        for (auto flow_id = 0; flow_id != num_flows; ++flow_id)
        {
            Flow& flow = flows[interface_id][flow_id];
            flow.mPacket.mData.resize(sizes[flow_id]);
            flow.set_mbps(mbps[flow_id]);
            bbInterfaces[interface_id].add_flow(flow);
        }
    }


    std::this_thread::sleep_for(std::chrono::seconds(10));

    physicalInterface.stop();

//    for (BBInterface& b : physicalInterface.getBBInterfaces())
//    {
//        b.print(&b - &physicalInterface.getBBInterfaces()[0]);
//    }
}
