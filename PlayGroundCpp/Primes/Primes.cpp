#include "Primes.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>


namespace Primes
{

    UInt32 FastSqrt(UInt32 n)
    {
       UInt32 root = 0, bit, trial;
       bit = (n >= 0x10000) ? 1<<30 : 1<<14;
       do
       {
          trial = root+bit;
          if (n >= trial)
          {
             n -= trial;
             root = trial+bit;
          }
          root >>= 1;
          bit >>= 2;
       } while (bit);
       return root;
    }


    bool IsPrime(UInt32 inNumber, const std::vector<UInt32> & inPrecedingPrimes)
    {
        UInt32 squaredNumber = FastSqrt(inNumber);
        for (size_t idx = 1; idx < inPrecedingPrimes.size(); ++idx)
        {
            const UInt32 & primeFactor = inPrecedingPrimes[idx];

            if (primeFactor > squaredNumber)
            {
                return true;
            }

            if (inNumber % primeFactor == 0)
            {
                return false;
            }
        }
        return true;
    }

    void GetPrimes(size_t n, std::vector<UInt32> & outPrimes)
    {
        if (!outPrimes.empty())
        {
            throw std::invalid_argument("outPrimes must be empty");
        }

        if (n < 1)
        {
            return;
        }

        outPrimes.push_back(2);
        
        for (UInt32 idx = 3; idx < n; idx += 2)
        {
            if (IsPrime(idx, outPrimes))
            {
                outPrimes.push_back(idx);
            }
        }
    }


    void FindPrimesInInterval(const Interval & inInterval,
                              const std::vector<UInt32> & inPrecedingPrimes,
                              std::vector<UInt32> & outPrimes)
    {
        size_t begin = inInterval.first;
        if (begin % 2 == 0)
        {
            begin++;
        }

        for (size_t idx = begin; idx < inInterval.second; idx += 2)
        {
            if (IsPrime(static_cast<UInt32>(idx), inPrecedingPrimes))
            {
                outPrimes.push_back(idx);
            }
        }
    }

    void Partition(size_t inNumberOfParts,
                   const Interval & inInterval,
                   std::vector<Interval> & outIntervals)
    {
        if (inInterval.first > inInterval.second)
        {
            throw std::logic_error("Bad interval");
        }
        else if (inInterval.first == inInterval.second)
        {
            return;
        }

        size_t range = inInterval.second - inInterval.first;
        size_t partSize = range / inNumberOfParts;
        size_t beginValue = inInterval.first;

        for (size_t idx = 0; idx != inNumberOfParts; ++idx)
        {
            if (idx < inNumberOfParts - 1)
            {
                outIntervals.push_back(std::make_pair(beginValue, beginValue + partSize));
                beginValue += partSize + 1;
            }
            else
            {
                if (beginValue > inInterval.second)
                {
                    throw std::logic_error("Last part too small. This should not happen.");
                }
                outIntervals.push_back(std::make_pair(beginValue, inInterval.second));
            }
        }

        // PRINT DEBUG
        //for (size_t idx = 0; idx != outIntervals.size(); ++idx)
        //{
        //    const Interval & interval = outIntervals[idx];
        //    std::cout << interval.first << "-" << interval.second << std::endl;
        //}
    }

    class Runner : public Poco::Runnable
    {
    public:
        Runner(const Interval & inInterval,
               const std::vector<UInt32> & inPrecedingPrimes) :
            mInterval(inInterval),
            mPrecedingPrimes(inPrecedingPrimes)
        {
        }

        virtual void run()
        {
            FindPrimesInInterval(mInterval, mPrecedingPrimes, mFoundPrimes);
        }

        const std::vector<UInt32> & foundPrimes() const
        {
            return mFoundPrimes;
        }

    private:
        Interval mInterval;
        std::vector<UInt32> mPrecedingPrimes;
        std::vector<UInt32> mFoundPrimes;
    };
    
    void FindPrimesInInterval_MultiThreaded(size_t inNumberOfThreads,
                                            const Interval & inInterval,
                                            const std::vector<UInt32> & inPrecedingPrimes,
                                            std::vector<UInt32> & outPrimes)
    {
        std::vector<Interval> intervals;
        Partition(inNumberOfThreads, inInterval, intervals);

        std::vector<Poco::Thread *> mThreads;
        std::vector<Runner*> runners;
        for (size_t idx = 0; idx < intervals.size(); ++idx)
        {            
            const Interval & interval = intervals[idx];
            mThreads.push_back(new Poco::Thread);
            runners.push_back(new Runner(interval, inPrecedingPrimes));
            mThreads.back()->start(*runners[idx]);
        }

        std::cout << "There are " << mThreads.size() << " threads running now." << std::endl;


        for (size_t idx = 0; idx != mThreads.size(); ++idx)
        {
            mThreads[idx]->join();
            const std::vector<UInt32> & primes = runners[idx]->foundPrimes();
            outPrimes.insert(outPrimes.end(), primes.begin(), primes.end());
            delete runners[idx];
            delete mThreads[idx];            
        }
    }

} // namespace Primes
