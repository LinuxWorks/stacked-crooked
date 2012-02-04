#ifndef RPC_CALL_H
#define RPC_CALL_H


#include "Client.h"
#include "RemoteObjects.h"
#include "Serialization.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <string>
#include <utility>


#if ENABLE_TRACE
#define TRACE std::cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << std::endl;
#else
#define TRACE
#endif


using boost::tuples::tuple;


template<typename Signature>
struct Decompose;


template<typename Ret_, typename Arg_>
struct Decompose<Ret_(Arg_)>
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
};


template<typename Signature_>
struct RemoteCall
{
    typedef Signature_ Signature;
    typedef RemoteCall<Signature> This;
    typedef typename Decompose<Signature>::Ret Ret;
    typedef typename Decompose<Signature>::Arg Arg;

    RemoteCall(const std::string & inName, const Arg & inArg) :
        mName(inName),
        mArg(inArg)
    {
    }

    const std::string & name() const { return mName; }

    const Arg & arg() const { return mArg; }

    #ifdef RPC_CLIENT
    Ret send()
    {
        return Redirector::Get().send(*this);
    }
    #endif // RPC_CLIENT

private:
    std::string mName;
    Arg mArg;
};


template<typename C,
         typename Arg_ = std::vector<typename C::Arg>,
         typename Ret_ = std::vector<typename C::Ret>,
         typename Base = RemoteCall<Ret_(Arg_)> >
struct RemoteBatchCall : public Base
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
    typedef RemoteBatchCall<C, Arg, Ret> This;

    static std::string Name() { return "RemoteBatchCall<" + C::Name() + ">"; }

#ifdef RPC_SERVER
    typedef typename C::Arg A;
    typedef typename C::Ret R;

    static std::vector<R> execute(const std::vector<A> & arg)
    {
        TRACE
        std::vector<R> result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            result.push_back(C::execute(A(arg[idx])));
        }
        return result;
    }
#endif

protected:
    RemoteBatchCall(const Arg & inArg) : Base(Name(), inArg) { }
};


template<typename C>
void Register();


template<typename C>
struct Batch;


//
// Helper macros for the RPC_CALL macro.
//
#define RPC_GENERATE_CALL(NAME, SIGNATURE) \
    struct NAME : public RemoteCall<SIGNATURE> { \
        typedef RemoteCall<SIGNATURE> Base; \
        typedef Base::Arg Arg; \
        typedef Base::Ret Ret; \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : RemoteCall<Ret(Arg)>(Name(), inArgs) { } \
        static Ret execute(const Arg & arg); \
    };

#define RPC_GENERATE_BATCH_CALL(NAME, SIGNATURE) \
    template<> \
    struct Batch<NAME> : public RemoteBatchCall<NAME> { \
        typedef RemoteBatchCall<NAME>::Arg Arg; \
        typedef RemoteBatchCall<NAME>::Ret Ret; \
        Batch(const Arg & args) : RemoteBatchCall<NAME>(args) { } \
    };


//
// The server can provide implementations for the RPC calls by
// implementating the "execute" method in the generated call.
//
#ifdef RPC_SERVER

#define RPC_RUN_ON_STARTUP(Statement) \
    namespace { struct __FILE__##"_"##__LINE { __FILE__##"_"##__LINE() { Statement } }; g##__FILE__##"_"##__LINE; }

#define RPC_REGISTER_BATCH_CALL(NAME) \
    struct Batch##NAME##Registrator { \
        Batch##NAME##Registrator() { Register< Batch<NAME> >(); } \
    }; \
    static Batch##NAME##Registrator g##Batch##NAME##Registrator;

#define RPC_CALL(NAME, SIGNATURE) \
    RPC_GENERATE_CALL(NAME, SIGNATURE) \
    RPC_RUN_ON_STARTUP(Register<NAME>();) \
    RPC_GENERATE_BATCH_CALL(NAME, SIGNATURE) \
    RPC_REGISTER_BATCH_CALL(NAME)

#else


/**
 * RPC_CALL macro for defining remote procedure calls.
 *
 * Technically the macro only takes two arguments: name and signature:
 *
 *     RPC_CALL(Name, Signature)
 *
 * The signature consists of a return value and one argument.
 * If you want multiple arguments you can use a container, tuple of struct.
 * You can emulate void with Void.
 *
 * Here's a simple RPC call for adding two numbers:
 *
 *     RPC_CALL(Add, int(tuple<int, int>))
 *
 * A class called Add is generated and is missing the implementation for the
 * execute() method. We have to provide the implementation:
 *
 *     int Add::execute(const tuple<int, int> & value)
 *     {
 *         return value.first + value.second;
 *     }
 *
 * That's it. Now we can try it out:
 *
 *     int seven = Add(make_tuple(3, 4)).send();
 *
 * There is also the Batch template which enables us to apply same method on
 * a vector of objects:
 *
 *     std::vector<tuple<int, int> > args = ...;
 *     std::vector<int> sums = Batch<Add>(args).send();
 *
 * Serialization works out-of-the box for builtin types, most standard containers,
 * boost tuple types and any combination of these. User defined structs and
 * classes must be made serializable. See the boost documentation for more info.
 */
#define RPC_CALL(NAME, SIGNATURE) \
    RPC_GENERATE_CALL(NAME, SIGNATURE) \
    RPC_GENERATE_BATCH_CALL(NAME, SIGNATURE)

#endif // RPC_SERVER


#endif // RPC_CALL_H
