#ifndef _POWEROBFUSCATORAPI_H
#define _POWEROBFUSCATORAPI_H

#include <types.h>
#include <stdint.h>
#include <string>
#undef vector
#include <vector>

#define POBF_API /* __attribute__((visibility("hidden"))) GCC Only */
#define START_PATTERN 0xAABBCCDD, 0x12345678, 0xEEFFEEFF 

namespace pobf
{
    struct opd_s
    {
        uint32_t func;
        uint32_t toc;
    };

    struct Pattern
    {
        const char* find;
        const char* mask;
        bool found;
    };

    namespace EncryptV1
    {
        extern char PRIV_API_KEY[];
        extern int EXPORTS_TOC[];

        extern void        POBF_API    SetApiKey(uint32_t key);
        extern uint32_t    POBF_API    MixTimeSeed(clock_t a, time_t b, sys_pid_t c);
        extern void        POBF_API    todo_SeedRandom(uint32_t seed);
        extern int         POBF_API    todo_Random();
        extern uint32_t    POBF_API    StringToHash32(const char* str);
        extern void        POBF_API    Start(int suppressParameter);
        extern void        POBF_API    DecryptAll();
        extern void        POBF_API    DecryptTextSegment(uint32_t function);
        extern void        POBF_API    DecryptDataSegment();
        extern bool        POBF_API    Skip(uint32_t instruction);
    }

    namespace EncryptV2
    {
        extern bool POBF_API DataCompare(const uint8_t* pbData, const uint8_t* pbMask, const char* szMask);
        extern bool POBF_API FindPattern(uintptr_t address, uint32_t length, uint8_t step, uint8_t* bytes, const char* mask, uint32_t* foundOffset);
        extern void POBF_API FindPatternsInParallel(uintptr_t address, uint32_t length, std::vector<Pattern>& patterns, std::vector<uint32_t>& foundOffsets);
        extern void POBF_API DecryptFunction(uint8_t* data, uint32_t startIndex, uint32_t endIndex, bool quick = false);

        namespace Default
        {
            extern void POBF_API __encryptFunctionStart(void* function, bool quick);
            extern void POBF_API _encryptFunctionStart(void* function, bool quick);
            extern void POBF_API __encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
            extern void POBF_API _encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
        }

        namespace Quick
        {
            extern void POBF_API __encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            extern void POBF_API _encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            extern void POBF_API _encryptFunctionEnd(uint8_t* saveBuffer, uint32_t start, uint32_t end);
        }

        namespace Inline
        {
            extern void POBF_API __encryptFunctionStart(void* function);
            extern void POBF_API _encryptFunctionStart(void* function);
            extern void POBF_API __encryptFunctionEnd(void* function);
            extern void POBF_API _encryptFunctionEnd(void* function);
        }
    }




    namespace Vx
    {
        // https://stackoverflow.com/questions/7270473/compile-time-string-encryption
        // http://www.rohitab.com/discuss/topic/39611-malware-related-compile-time-hacks-with-c11/

        //-------------------------------------------------------------//
        // "Malware related compile-time hacks with C++11" by LeFF	 //
        // You can use this code however you like, I just don't really //
        // give a shit, but if you feel some respect for me, please	//
        // don't cut off this comment when copy-pasting... ;-)		 //
        //-------------------------------------------------------------//

        #ifdef _WINDOWS
        #define ALWAYS_INLINE __forceinline
        #else
        #define ALWAYS_INLINE __attribute__((always_inline))
        #endif


        #ifndef vxCPLSEED
        // If you don't specify the seed for algorithms, the time when compilation
        // started will be used, seed actually changes the results of algorithms...
        #define vxCPLSEED ((__TIME__[7] - '0') * 1	+ (__TIME__[6] - '0') * 10  + \
                    (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + \
                    (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000)
        #endif

        // The constantify template is used to make sure that the result of constexpr
        // function will be computed at compile-time instead of run-time
        template <uint32_t Const> struct vxCplConstantify { enum { Value = Const }; };

        // Compile-time mod of a linear congruential pseudorandom number generator,
        // the actual algorithm was taken from "Numerical Recipes" book
        constexpr uint32_t vxCplRandom(uint32_t Id)
        {
            return (1013904223 + 1664525 * ((Id > 0) ? (vxCplRandom(Id - 1)) : (vxCPLSEED))) & 0xFFFFFFFF;
        }

        // Compile-time random macros, can be used to randomize execution  
        // path for separate builds, or compile-time trash code generation
        #define vxRAND()		   (pobf::Vx::vxCplConstantify<pobf::Vx::vxCplRandom(__COUNTER__ + 1)>::Value)
        #define vxRANDOM(Min, Max) (Min + (vxRAND() % (Max - Min + 1)))
        

        // Compile-time recursive mod of string hashing algorithm,
        // the actual algorithm was taken from Qt library (this
        // function isn't case sensitive due to vxCplTolower)
        constexpr char	 vxCplTolower(char Ch) { return (Ch >= 'A' && Ch <= 'Z') ? (Ch - 'A' + 'a') : (Ch); }
        constexpr uint32_t vxCplHashPart3(char Ch, uint32_t Hash) { return ((Hash << 4) + vxCplTolower(Ch)); }
        constexpr uint32_t vxCplHashPart2(char Ch, uint32_t Hash) { return (vxCplHashPart3(Ch, Hash) ^ ((vxCplHashPart3(Ch, Hash) & 0xF0000000) >> 23)); }
        constexpr uint32_t vxCplHashPart1(char Ch, uint32_t Hash) { return (vxCplHashPart2(Ch, Hash) & 0x0FFFFFFF); }
        constexpr uint32_t vxCplHash(const char* Str) { return (*Str) ? (vxCplHashPart1(*Str, vxCplHash(Str + 1))) : (0); }

        // Compile-time generator for list of indexes (0, 1, 2, ...)
        template <uint32_t...> struct vxCplIndexList {};
        template <typename	IndexList, uint32_t Right> struct vxCplAppend;
        template <uint32_t... Left, uint32_t Right> struct vxCplAppend<vxCplIndexList<Left...>, Right> { typedef vxCplIndexList<Left..., Right> Result; };
        template <uint32_t N> struct vxCplIndexes { typedef typename vxCplAppend<typename vxCplIndexes<N - 1>::Result, N - 1>::Result Result; };
        template <> struct vxCplIndexes<0> { typedef vxCplIndexList<> Result; };

        // Compile-time string encryption of a single character
        const unsigned char vxCplEncryptCharKey = vxRANDOM(0, 0xFF);
        constexpr char vxCplEncryptChar(const char Ch, uint32_t Idx) { return Ch ^ (vxCplEncryptCharKey + Idx); }

        // Compile-time string encryption class
        template <typename IndexList> struct vxCplEncryptedString;
        template <uint32_t... Idx>	struct vxCplEncryptedString<vxCplIndexList<Idx...> >
        {
            char Value[sizeof...(Idx) + 1]; // Buffer for a string

            // Compile-time constructor
            constexpr inline vxCplEncryptedString(const char* const Str)
                : Value{ vxCplEncryptChar(Str[Idx], Idx)... } {}

            // Run-time decryption
            char* decrypt()
            {
                for (uint32_t t = 0; t < sizeof...(Idx); t++)
                {
                    this->Value[t] = this->Value[t] ^ (vxCplEncryptCharKey + t);
                }
                this->Value[sizeof...(Idx)] = '\0';
                return this->Value;
            }
        };
    }

    namespace ADVobfuscator
    {
        // https://github.com/andrivet/ADVobfuscator


        // std::index_sequence will be available with C++14 (C++1y). For the moment, implement a (very) simplified and partial version. You can find more complete versions on the Internet
        // MakeIndex<N>::type generates Indexes<0, 1, 2, 3, ..., N>
        template<int... I>
        struct Indexes { using type = Indexes<I..., sizeof...(I)>; };

        template<int N>
        struct Make_Indexes { using type = typename Make_Indexes<N - 1>::type::type; };

        template<>
        struct Make_Indexes<0> { using type = Indexes<>; };





        // Very simple compile-time random numbers generator.

        // For a more complete and sophisticated example, see:
        // http://www.researchgate.net/profile/Zalan_Szgyi/publication/259005783_Random_number_generator_for_C_template_metaprograms/file/e0b49529b48272c5a6.pdf

        namespace
        {
            // I use current (compile time) as a seed

            constexpr char time[] = __TIME__; // __TIME__ has the following format: hh:mm:ss in 24-hour time

            // Convert time string (hh:mm:ss) into a number
            constexpr int DigitToInt(char c) { return c - '0'; }
            const int seed = DigitToInt(time[7]) +
                DigitToInt(time[6]) * 10 +
                DigitToInt(time[4]) * 60 +
                DigitToInt(time[3]) * 600 +
                DigitToInt(time[1]) * 3600 +
                DigitToInt(time[0]) * 36000;
        }

        // 1988, Stephen Park and Keith Miller
        // "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
        // Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimization:
        // with 32-bit math and without division

        template<int N>
        struct MetaRandomGenerator
        {
        private:
            static constexpr unsigned a = 16807;        // 7^5
            static constexpr unsigned m = 2147483647;   // 2^31 - 1

            static constexpr unsigned s = MetaRandomGenerator<N - 1>::value;
            static constexpr unsigned lo = a * (s & 0xFFFF);                // Multiply lower 16 bits by 16807
            static constexpr unsigned hi = a * (s >> 16);                   // Multiply higher 16 bits by 16807
            static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);     // Combine lower 15 bits of hi with lo's upper bits
            static constexpr unsigned hi2 = hi >> 15;                       // Discard lower 15 bits of hi
            static constexpr unsigned lo3 = lo2 + hi;

        public:
            static constexpr unsigned max = m;
            static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
        };

        template<>
        struct MetaRandomGenerator<0>
        {
            static constexpr unsigned value = seed;
        };

        // Note: A bias is introduced by the modulo operation.
        // However, I do believe it is neglictable in this case (M is far lower than 2^31 - 1)

        template<int N, int M>
        struct MetaRandom
        {
            static const int value = MetaRandomGenerator<N + 1>::value % M;
        };








        // Represents an obfuscated string, parametrized with an algorithm number N, a list of indexes Indexes and a key Key

        template<int N, char Key, typename Indexes>
        struct MetaString;

        // Partial specialization with a list of indexes I, a key K and algorithm N = 0
        // Each character is encrypted (XOR) with the same key

        template<char K, int... I>
        struct MetaString<0, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time.
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : key_{ K }, buffer_{ encrypt(str[I], K)... } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i]);
                buffer_[sizeof...(I)] = 0;
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            constexpr char key() const { return key_; }
            constexpr char ALWAYS_INLINE encrypt(char c, int k) const { return c ^ k; }
            constexpr char decrypt(char c) const { return encrypt(c, key()); }

            volatile int key_; // key. "volatile" is important to avoid uncontrolled over-optimization by the compiler
            volatile char buffer_[sizeof...(I) + 1]; // Buffer to store the encrypted string + terminating null byte
        };

        // Partial specialization with a list of indexes I, a key K and algorithm N = 1
        // Each character is encrypted (XOR) with an incremented key.

        template<char K, int... I>
        struct MetaString<1, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time.
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : key_(K), buffer_{ encrypt(str[I], I)... } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i], i);
                buffer_[sizeof...(I)] = 0;
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            constexpr char key(size_t position) const { return static_cast<char>(key_ + position); }
            constexpr char ALWAYS_INLINE encrypt(char c, size_t position) const { return c ^ key(position); }
            constexpr char decrypt(char c, size_t position) const { return encrypt(c, position); }

            volatile int key_; // key. "volatile" is important to avoid uncontrolled over-optimization by the compiler
            volatile char buffer_[sizeof...(I) + 1]; // Buffer to store the encrypted string + terminating null byte
        };

        // Partial specialization with a list of indexes I, a key K and algorithm N = 2
        // Shift the value of each character and does not store the key. It is only used at compile-time.

        template<char K, int... I>
        struct MetaString<2, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time. Key is *not* stored
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : buffer_{ encrypt(str[I])..., 0 } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i]);
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            // Be sure that the encryption key is never 0.
            constexpr char key(char key) const { return 1 + (key % 13); }
            constexpr char ALWAYS_INLINE encrypt(char c) const { return c + key(K); }
            constexpr char decrypt(char c) const { return c - key(K); }

            // Buffer to store the encrypted string + terminating null byte. Key is not stored
            volatile char buffer_[sizeof...(I) + 1];
        };

        // Helper to generate a key
        template<int N>
        struct MetaRandomChar
        {
            // Use 0x7F as maximum value since most of the time, char is signed (we have however 1 bit less of randomness)
            static const char value = static_cast<char>(1 + MetaRandom<N, 0x7F - 1>::value);
        };
    }

    namespace Enstone
    {
        // Compile-time generator for random numbers
        template<int N>
        struct RandomValueGenerator
        {
        private:
            static constexpr int s = RandomValueGenerator<N - 1>::value;
            static constexpr unsigned int lo = 16807 * (s & 0xFFFF);
            static constexpr unsigned int hi = 16807 * (s >> 16);
            static constexpr unsigned int res = lo + ((hi & 0x7FFF) << 16) + hi;

        public:
            static constexpr int value = res > INT_MAX ? res - INT_MAX : res;
        };

        template<>
        struct RandomValueGenerator<0>
        {
            // Convert compilation time to a seed if N is equal to 0
            static constexpr int value =
                __TIME__[7] - '0' +
                (__TIME__[6] - '0') * 10 +
                (__TIME__[4] - '0') * 60 +
                (__TIME__[3] - '0') * 600 +
                (__TIME__[1] - '0') * 3600 +
                (__TIME__[0] - '0') * 36000;
        };

        template<int n, int min, int max>
        struct RandomValue
        {
            static constexpr int value = min + RandomValueGenerator<n + 1>::value % (max - min);
        };


        // Compile-time generator for list of indexes (0, 1, 2, ...)
        template <int...>
        struct IndexList
        {
        };

        template <typename	List, int Right>
        struct Append;

        template <int... Left, int Right>
        struct Append<IndexList<Left...>, Right>
        {
            typedef IndexList<Left..., Right> Result;
        };

        template <int N>
        struct MakeIndexes
        {
            typedef typename Append<typename MakeIndexes<N - 1>::Result, N - 1>::Result Result;
        };

        template <>
        struct MakeIndexes<0>
        {
            typedef IndexList<> Result;
        };


        // Run-time decryption function | Reversed engineered from CCAPI by @jordywastaken
        static std::string DecryptString(const char* in, int len, int key)
        {
            std::string out = std::string(len, '\0');

            char previous = 0;
            for (int i = len - 1; i >= 0; --i)
            {
                char v16 = i + key + previous;
                previous = out[i] = v16 ^ in[i];
            }
            return out;
        }


        // Compile-time string encryption class
        template <int key, int len, typename List>
        struct Encrypted;

        template <int _key, int _len, int... I>
        struct Encrypted<_key, _len, IndexList<I...> >
        {
            char data[_len];

            // Compile-time constructor
            constexpr inline Encrypted(const char* const Str)
                : data{ encrypt(Str[I], (I < _len - 1) ? Str[I + 1] : 0, I, _key)... } {}

            constexpr char encrypt(char current, char next, int i, int key)
            {
                return current ^ char((i + key + next));
            }
        };
    }

    namespace MetaRand
    {
        // Meta Random Integer
        // https://github.com/cr-lupin/metarand

        constexpr char meta_rand_inits[] = __TIME__;
        const int meta_rand_seed = (meta_rand_inits[0] - '0') * 100000 + (meta_rand_inits[1] - '0') * 10000 +
            (meta_rand_inits[3] - '0') * 1000 + (meta_rand_inits[4] - '0') * 100 + (meta_rand_inits[6] - '0') * 10 + meta_rand_inits[7] - '0';

        template<typename E>
        struct CMetaUInt_eval
        {
            typedef typename E::type type;
        };

        template<typename E>
        struct CMetaUInt_init
        {
            typedef E type;
        };

        template<typename Engine>
        struct CMetaUInt_Random
        {
            //static const uint_fast32_t value = seed;
            //typedef linear_congruential_engine<uint_fast32_t, value> type;
            typedef typename CMetaUInt_init<Engine>::type type;
            static const decltype(type::value) value = type::value;
        };

        template<typename R>
        struct CMetaUInt_Next
        {
            typedef typename CMetaUInt_eval<R>::type type;
            static const decltype(type::value) value = type::value;
            //static const uint_fast32_t value = linear_congruential_engine<R>::value; //next random val
        };


        //--------------- linear_congruential_engine ---------------------

        template<typename UIntType,
            UIntType seed = meta_rand_seed,
            UIntType a = 16807,
            UIntType c = 0,
            UIntType m = 2147483647>
        struct linear_congruential_engine
        {
            static const UIntType value = seed;
            static const UIntType maxvalue = m - 1;
        };

        template<typename UIntType, UIntType seed, UIntType a, UIntType c, UIntType m>
        struct CMetaUInt_eval<linear_congruential_engine<UIntType, seed, a, c, m>>
        {
            static const UIntType value = (a * seed + c) % m;
            typedef linear_congruential_engine<
                UIntType,
                (a* seed + c) % m,
                a,
                c,
                m
            > type;
        };

        template<typename UIntType,
            UIntType seed,
            UIntType a,
            UIntType c,
            UIntType m>
        struct CMetaUInt_init<linear_congruential_engine<UIntType, seed, a, c, m>>
        {
            typedef typename CMetaUInt_eval<linear_congruential_engine<UIntType, seed, a, c, m>>::type type;
            static const UIntType value = type::value;
        };

        // ----------------- fib_engine ----------------------------

        template<typename UIntType,
            UIntType a = 0,
            UIntType b = 1>
        struct fib_engine
        {
            static const UIntType value = a;
        };

        template<typename UIntType, UIntType a, UIntType b>
        struct CMetaUInt_eval<fib_engine<UIntType, a, b> >
        {
            static const UIntType value = a + b;
            typedef fib_engine<
                UIntType,
                b,
                value
            > type;
        };

        // ------------------------ substract_with_carry --------------

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            UIntType seed = meta_rand_seed>
        struct substract_with_carry_engine
        {
        };

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            UIntType res,
            UIntType... vals>
        struct substract_with_carry_impl
        {
            static const UIntType value = res;

            template< unsigned int N, UIntType v, UIntType... vs >
            struct extract
            {
                const static UIntType value = extract<N - 1, vs...>::value;
            };

            template< UIntType v, UIntType... vs  >
            struct extract<0, v, vs...>
            {
                const static UIntType value = v;
            };

            template< unsigned int N >
            struct get
            {
                const static UIntType value = extract<N, vals...>::value;
            };

            //debug
            template< UIntType v, UIntType... vs>
            struct print_aux
            {
                static void print()
                {
                    //printf("0x%X ", v);
                    print_aux<vs...>::print();
                }
            };

            template< UIntType v>
            struct print_aux<v>
            {
                static void print()
                {
                    //printf("0x%X\n", v);
                }
            };

            struct print
            {
                static void dump()
                {
                    print_aux<vals...>::print();
                }
            };

        };

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            typename FillRnd, unsigned int cnt, UIntType... vals>
        struct fillrnd
        {
            typedef typename CMetaUInt_Next<FillRnd>::type RND;
            typedef typename fillrnd<UIntType,
                r, s, m,
                RND, cnt - 1, vals..., RND::value >::type type;
        };

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            typename FillRnd, UIntType... vals>
        struct fillrnd<UIntType, r, s, m, FillRnd, 0, vals...>
        {
            typedef substract_with_carry_impl< UIntType,
                r, s, m, 0, //0 is dummy result
                vals...> type;
        };

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            UIntType seed>
        struct CMetaUInt_init<substract_with_carry_engine<UIntType, r, s, m, seed>>
        {
            typedef typename fillrnd<
                UIntType,
                r, s, m,
                typename CMetaUInt_Random<linear_congruential_engine<uint_fast32_t, seed>>::type,
                r
            >::type type;
        };

        template<typename UIntType,
            UIntType r,
            UIntType s,
            UIntType m,
            UIntType res,
            UIntType val,
            UIntType... vals>
        struct CMetaUInt_eval<substract_with_carry_impl<UIntType, r, s, m, res, val, vals...>>
        {
            //typedef seq<UIntType, vals...> X;
            typedef substract_with_carry_impl<UIntType, r, s, m, res, val, vals...> X;
            static const UIntType value = (X::template get<0>::value - X:: template get<r - s>::value);
            //static const UIntType value = val + 1;
            typedef substract_with_carry_impl<
                UIntType,
                r, s, m,
                value,
                vals...,
                value
            > type;
        };

        // ------------------------------- Bernoulli

        template<typename Engine, int N, int D, bool val = false>
        struct Bernoulli
        {
            static const bool value = val;
        };

        template<typename Engine, int N, int D, bool b>
        struct CMetaUInt_eval<Bernoulli<Engine, N, D, b>>
        {
            typedef typename CMetaUInt_Next<Engine>::type tmptype;
            static const bool value = static_cast<double>(tmptype::value) / tmptype::maxvalue < static_cast<double>(N) / D;
            typedef Bernoulli<tmptype, N, D, value> type;

        };

        template<int cnt, typename R>
        struct CMetaUInt_PrintRandoms
        {
            static void print()
            {
                typedef typename CMetaUInt_Next<R>::type RND;
                //printf("0x%X ", RND::value);
                CMetaUInt_PrintRandoms<cnt - 1, RND >::print();
            }
        };

        template<typename R>
        struct CMetaUInt_PrintRandoms<0, R>
        {
            static void print()
            {
                //printf("0x%X \n", CMetaUInt_Next<R>::value);
            }
        };


        typedef CMetaUInt_init<substract_with_carry_engine<unsigned int, 10, 2, 1>>::type X;
        typedef CMetaUInt_Next<X>::type RandomCompileTimeInteger1;
        typedef CMetaUInt_Next<RandomCompileTimeInteger1>::type RandomCompileTimeInteger2;
        typedef CMetaUInt_Next<RandomCompileTimeInteger2>::type RandomCompileTimeInteger3;
        typedef CMetaUInt_Next<RandomCompileTimeInteger3>::type RandomCompileTimeInteger4;
        typedef CMetaUInt_Next<RandomCompileTimeInteger4>::type RandomCompileTimeInteger5;
        typedef CMetaUInt_Next<RandomCompileTimeInteger5>::type RandomCompileTimeInteger6;
        typedef CMetaUInt_Next<RandomCompileTimeInteger6>::type RandomCompileTimeInteger7;
        typedef CMetaUInt_Next<RandomCompileTimeInteger7>::type RandomCompileTimeInteger8;

    }



}



/************* VxObfuscator *******************/

// Compile-time hashing macro, hash values changes using the first pseudorandom number in sequence
#define vxHASH(Str) (uint32_t)(pobf::Vx::vxCplConstantify<pobf::Vx::vxCplHash(Str)>::Value ^ pobf::Vx::vxCplConstantify<pobf::Vx::vxCplRandom(1)>::Value)

// Compile-time string encryption macro
// NOTE(Roulette): In disassembly PPC64 it uses memcpy and a small for loop to decrypt the string
#define vxENCRYPT(Str) (pobf::Vx::vxCplEncryptedString<pobf::Vx::vxCplIndexes<sizeof(Str) - 1>::Result>(Str).decrypt())
#define ENCRYPTSTRV1(str) vxENCRYPT(str)



/************* ADVobfuscator *******************/

// Prefix notation
// NOTE(Roulette): In disassembly PPC64 it does not use memcpy but rather individual instructions like "li r3 0x48" for each byte which increases code size (sprx size increase for each use case)
#define ADV_DEF_OBFUSCATED(str) pobf::ADVobfuscator::MetaString<pobf::ADVobfuscator::MetaRandom<__COUNTER__, 3>::value, pobf::ADVobfuscator::MetaRandomChar<__COUNTER__>::value, pobf::ADVobfuscator::Make_Indexes<sizeof(str) - 1>::type>(str)
#define ADV_OBFUSCATED_STR(str) (ADV_DEF_OBFUSCATED(str).decrypt())

#define ENCRYPTSTRV2(str) ADV_OBFUSCATED_STR(str)



/************* EnstoneObfuscator *******************/

// Disable warning 1724 'routine is both "inline" and "noinline"'
#pragma diag_suppress 1724

// NOTE(Roulette): This string encryption "may" be slightly slower since it uses std::string. Need to profile it to actually see results.
#define EnstoneStringEncrypt(str) []() __attribute__((noinline)) \
    { \
        constexpr int key = pobf::Enstone::RandomValue<__COUNTER__, -200, 200>::value; \
        static constexpr auto encrypted = pobf::Enstone::Encrypted<key, sizeof(str), pobf::Enstone::MakeIndexes<sizeof(str)>::Result>(str); \
        return pobf::Enstone::DecryptString(encrypted.data, sizeof(str), key); \
    }().c_str()

#define ENCRYPTSTRV3(str) EnstoneStringEncrypt(str)




        /******* DefaultEncrypt ********/
#define encryptFunctionStart(function) \
            void *local_encrypt_function_address = function; \
            pobf::EncryptV2::Default::_encryptFunctionStart(local_encrypt_function_address, false);

#define encryptFunctionEnd() \
            pobf::EncryptV2::Default::_encryptFunctionEnd(local_encrypt_function_address, false);

#define encryptFunctionEnd_deletedata() \
            pobf::EncryptV2::Default::_encryptFunctionEnd(local_encrypt_function_address, false, true);


        /******* QuickEncrypt ********/
#define encryptFunctionStart_quick(function, bufferSize) \
            void *local_encrypt_function_address = function; \
            static uint8_t encryptionBuffer[bufferSize]; \
            static uint32_t startEncryptAddress = 0; \
            static uint32_t endEncryptAddress = 0; \
            pobf::EncryptV2::Quick::_encryptFunctionStart(function, encryptionBuffer, &startEncryptAddress, &endEncryptAddress);

#define encryptFunctionEnd_quick() \
            pobf::EncryptV2::Quick::_encryptFunctionEnd(encryptionBuffer, startEncryptAddress, endEncryptAddress);


        /******* InlineEncrypt ********/
#define inline_encryptFunctionStart(function) \
            void *local_encrypt_function_address = function; \
            pobf::EncryptV2::Inline::_encryptFunctionStart(local_encrypt_function_address);

#define inline_encryptFunctionEnd() \
            pobf::EncryptV2::Inline::_encryptFunctionEnd(local_encrypt_function_address);


        /******* For pattern scanning ********/
#define StartatternDefault() \
            __asm("xor %r3, %r3, %r4;"       /* \x7C\x63\x22\x78 */    \
                    "xor %r3, %r3, %r4;");   /* \x7C\x63\x22\x78 */

#define EndPatternDefault() \
            __asm("xor %r3, %r3, %r5;"       /* \x7C\x63\x2A\x78 */   \
                    "xor %r3, %r3, %r5;");   /* \x7C\x63\x2A\x78 */

#define StartPatternQuick() \
            __asm("xor %r3, %r3, %r6;"       /* \x7C\x63\x32\x78 */   \
                    "xor %r3, %r3, %r6;");   /* \x7C\x63\x32\x78 */

#define EndPatternQuick() \
            __asm("xor %r3, %r3, %r7;"       /* \x7C\x63\x3A\x78 */   \
                    "xor %r3, %r3, %r7;");   /* \x7C\x63\x3A\x78 */

#define StartPattternThread() \
            __asm("lis %r3, 0xFFEE;"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCC;"); /* \x60\x63\xDD\xCC */

#define EndPatternThread() \
            __asm("lis %r3, 0xFFEE;"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCD;"); /* \x60\x63\xDD\xCD */




#endif // _POWEROBFUSCATORAPI_H 