#include <boost/circular_buffer.hpp>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <stdint.h>


std::vector<uint8_t> generateTestData()
{
    std::vector<uint8_t> data;
    data.reserve(1000);
    while (data.size() != data.capacity())
    {
        data.push_back(data.size());
    }
    return data;
}


void print(std::string str)
{
    static std::mutex fMutex;
    std::lock_guard<std::mutex> lock(fMutex);
    std::cout << str << '\n';
}


inline std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& vec)
{
    for (auto& el : vec)
    {
        os << std::setw(2) << std::setfill('0') << static_cast<int>(el) << ' ';
    }
    return os;
}


class MakeString
{
public:
    template <typename T>
    MakeString & operator<<(const T & datum)
    {
        mBuffer << datum;
        return *this;
    }

    MakeString & operator<<(uint8_t n)
    {
        mBuffer << std::setw(2) << std::setfill('0') << static_cast<int>(n) << ' ';
        return *this;
    }

    operator std::string() const
    { return mBuffer.str(); }

    std::string str() const
    { return mBuffer.str(); }

private:
    std::ostringstream mBuffer;
};


struct ConcurrentBuffer
{
    ConcurrentBuffer(std::size_t inSize) :
        mBuffer(inSize),
        mNumRead(0),
        mNumWritten(0)
    {
    }

    std::size_t size() const
    {
        return mBuffer.size();
    }

    bool empty() const
    {
        return mBuffer.empty();
    }

    std::size_t read(uint8_t* buf, std::size_t len)
    {
        // first make a copy of the number of written bytes.
        std::size_t numWritten = mNumWritten;

        assert(numWritten >= mNumRead);

        // number of bytes availalbe for reading is equal to the total number of
        // written bytes subtracted with the number of already read bytes
        // with the input length as an upper bound
        auto available = std::min(len, numWritten - mNumRead);


        if (available == 0)
        {
            return 0;
        }

        auto begin = mBuffer.begin() + mNumRead;
        auto end = begin + available;

        for (; begin != end;)
        {
            *buf++ = *begin++;
        }

        // this enables writer thread to start writing again
        mNumRead += available;
        return available;
    }

    std::size_t write(uint8_t* buf, std::size_t len)
    {
        std::size_t numRead = mNumRead;

        assert(mNumWritten <= numRead);

        // the number of bytes available for writing is equal to the the size
        // of the buffer plus the  of bytes written in the past mins the number
        // of read bytes in the  past
        // the given length is an upper limit
        auto availableSpace = std::min(len, mBuffer.size() + mNumWritten - numRead);
        if (availableSpace == 0)
        {
            return 0;
        }


        auto begin = mBuffer.begin() + mNumWritten;
        auto end = begin + availableSpace;
        for (; begin != end; )
        {
            *begin++ = *buf++;
        }

        mNumWritten += availableSpace;
        return availableSpace;
    }


    boost::circular_buffer<uint8_t> mBuffer;
    std::atomic<std::size_t> mNumRead;
    std::atomic<std::size_t> mNumWritten;
};


int main()
{
    print("Creating the ConcurrentBuffer");
    ConcurrentBuffer buf(100);

    print("Generate the test data");
    auto testData = generateTestData();

    print("Start the reader thread");
    std::thread([&] {
        print("Inside the reader thread");
        std::vector<uint8_t> receiveData(testData.size());
        auto numReceived = 0UL;
        while (numReceived != testData.size()) {
            if (buf.empty()) {
                print("READER: buffer is empty vOv");
                sleep(1);
                continue;
            }

            numReceived += buf.read(
                &receiveData[0] + numReceived,
                receiveData.size() - numReceived);
            if (numReceived != 0) {
                print(MakeString() << "READER: numReceived=" << numReceived);
            }
        }
        print(MakeString() << "testData=" << testData);
        print(MakeString() << "receiveData=" << receiveData);
        assert(testData == receiveData);

    }).detach();

    print("Start writing in the main thread.");
    auto numWritten = 0UL;
    while (numWritten != testData.size()) {
        if (auto n = buf.write(
                testData.data() + numWritten,
                testData.size() - numWritten)) {

            print(MakeString() << "n=" << n << " numWritten" << numWritten << " size=" << testData.size());
            if (numWritten == testData.size()) {
                print("write buffer is complete.");
                break;
            }
        } else {
            print(MakeString() << "writer: write() returned 0, numWritten=" << numWritten);
            sleep(1);
        }
    }
}
