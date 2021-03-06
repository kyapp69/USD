//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef HD_PERF_H
#define HD_PERF_H

#include "pxr/imaging/hd/version.h"
#include "pxr/imaging/hd/debugCodes.h"

#include "pxr/base/tracelite/trace.h"

#include "pxr/base/tf/mallocTag.h"
#include "pxr/base/tf/singleton.h"
#include "pxr/base/tf/token.h"

#include <boost/noncopyable.hpp>
#include "pxr/base/tf/hashmap.h"

#include <mutex>

class SdfPath;

// XXX: it would be nice to move this into Trace or use the existing Trace
// counter mechanism, however we are restricted to TraceLite in the rocks.

//----------------------------------------------------------------------------//
// PERFORMANCE INSTURMENTATION MACROS                                         //
//----------------------------------------------------------------------------//

// Emits a trace scope tagged for the function.
#define HD_TRACE_FUNCTION() TRACE_FUNCTION()
// Emits a trace scope with the specified tag.
#define HD_TRACE_SCOPE(tag) TRACE_SCOPE(tag)

// Creates an auto-mallocTag with the function, including template params.
#define HD_MALLOC_TAG_FUNCTION() \
    TfAutoMallocTag2 tagFunc("Hd", __PRETTY_FUNCTION__);
// Creates an auto-mallocTag with the given named tag.
#define HD_MALLOC_TAG(x) \
    TfAutoMallocTag2 tag2("Hd", x);

// Overrides operator new/delete and injects malloc tags.
#define HD_MALLOC_TAG_NEW(x) \
    TF_MALLOC_TAG_NEW("Hd", x);

// Adds a cache hit for the given cache name, the id is provided for debugging,
// see HdPerfLog for details.
#define HD_PERF_CACHE_HIT(name, id) \
    HdPerfLog::GetInstance().AddCacheHit(name, id);
#define HD_PERF_CACHE_HIT_TAG(name, id, tag) \
    HdPerfLog::GetInstance().AddCacheHit(name, id, tag);

// Adds a cache miss for the given cache name, the id is provided for debugging,
// see HdPerfLog for details.
#define HD_PERF_CACHE_MISS(name, id) \
    HdPerfLog::GetInstance().AddCacheMiss(name, id);
#define HD_PERF_CACHE_MISS_TAG(name, id, tag) \
    HdPerfLog::GetInstance().AddCacheMiss(name, id, tag);

// Increments/Decrements/Sets/Adds/Subtracts a named performance counter
// see HdPerfLog for details.
#define HD_PERF_COUNTER_INCR(name) \
    HdPerfLog::GetInstance().IncrementCounter(name);
#define HD_PERF_COUNTER_DECR(name) \
    HdPerfLog::GetInstance().DecrementCounter(name);
#define HD_PERF_COUNTER_SET(name, value) \
    HdPerfLog::GetInstance().SetCounter(name, value);
#define HD_PERF_COUNTER_ADD(name, value) \
    HdPerfLog::GetInstance().AddCounter(name, value);
#define HD_PERF_COUNTER_SUBTRACT(name, value) \
    HdPerfLog::GetInstance().SubtractCounter(name, value);

//----------------------------------------------------------------------------//
// PERFORMANCE LOG                                                            //
//----------------------------------------------------------------------------//

/// Performance counter monitoring.
///
class HdPerfLog : public boost::noncopyable {
public:

    static HdPerfLog& GetInstance() {
        return TfSingleton<HdPerfLog>::GetInstance();
    }

    /// Tracks a cache hit for the named cache, the id and tag are reported
    /// when debug logging is enabled.
    void AddCacheHit(TfToken const& name,
                     SdfPath const& id,
                     TfToken const& tag=TfToken());

    /// Tracks a cache miss for the named cache, the id and tag are reported
    /// when debug logging is enabled.
    void AddCacheMiss(TfToken const& name,
                      SdfPath const& id,
                      TfToken const& tag=TfToken());

    void ResetCache(TfToken const& name);

    /// Gets the hit ratio (numHits / totalRequests) of a cache performance
    /// counter.
    double GetCacheHitRatio(TfToken const& name);

    /// Gets the number of hit hits for a cache performance counter.
    size_t GetCacheHits(TfToken const& name);

    /// Gets the number of hit misses for a cache performance counter.
    size_t GetCacheMisses(TfToken const& name);

    /// Returns the names of all cache performance counters.
    TfTokenVector GetCacheNames();

    /// Returns a vector of all performance counter names.
    TfTokenVector GetCounterNames();

    /// Increments a named counter by 1.0.
    void IncrementCounter(TfToken const& name);

    /// Decrements a named counter by 1.0.
    void DecrementCounter(TfToken const& name);

    /// Sets the value of a named counter.
    void SetCounter(TfToken const& name, double value);

    /// Adds value to a named counter.
    void AddCounter(TfToken const& name, double value);

    /// Subtracts value to a named counter.
    void SubtractCounter(TfToken const& name, double value);

    /// Returns the current value of a named counter.
    double GetCounter(TfToken const& name);

    /// Reset all conter values to 0.0. Note that this doesn't reset cache counters.
    void ResetCounters();

    /// Enable performance logging.
    void Enable() { _enabled = true; }

    /// Disable performance logging.
    void Disable() { _enabled = false; }

private:
    friend class TfSingleton<HdPerfLog>;
    HdPerfLog();
    ~HdPerfLog();

    // Tracks number of hits and misses and provides some convenience API.
    class _CacheEntry {
    public:
        _CacheEntry() : _hits(0), _misses(0) { }

        void AddHit() {++_hits;}
        size_t GetHits() {return _hits;}

        void AddMiss() {++_misses;}
        size_t GetMisses() {return _misses;}

        size_t GetTotal() {return _hits+_misses;}
        double GetHitRatio() {return (double)_hits / GetTotal();}

        void Reset() { _hits = 0; _misses = 0; }
    private:
        size_t _hits;
        size_t _misses;
    };

    // Cache performance counters.
    typedef TfHashMap<TfToken, _CacheEntry, TfToken::HashFunctor> _CacheMap;
    _CacheMap _cacheMap;

    // Named value counters.
    typedef TfHashMap<TfToken, double, TfToken::HashFunctor> _CounterMap;
    _CounterMap _counterMap;

    // Enable / disable performance tracking.
    bool _enabled;
    std::mutex _mutex;
    typedef std::lock_guard<std::mutex> _Lock;
};

#endif // HD_PERF_H
