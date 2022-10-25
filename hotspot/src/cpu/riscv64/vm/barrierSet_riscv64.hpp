/*
 * Copyright (c) 2000, 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_MEMMORY_BARRIERSET_RISCV64_HPP
#define SHARE_VM_MEMMORY_BARRIERSET_RISCV64_HPP

#include "barrierSetConfig_riscv64.hpp"
#include "memory/memRegion.hpp"
//#include "oops/access.hpp"
//#include "oops/accessBackend.hpp"
#include "oops/oopsHierarchy.hpp"
#include "utilities/macros.hpp"

class BarrierSetAssembler;
class JavaThread;

// This class provides the interface between a barrier implementation and
// the rest of the system.

class BarrierSetRv: public CHeapObj<mtGC> {
  friend class VMStructs;

  static BarrierSetRv* _barrier_set;

public:
  enum Name {
#define BARRIER_SET_DECLARE_BS_ENUM(bs_name) bs_name ,
    FOR_EACH_BARRIER_SET_DO(BARRIER_SET_DECLARE_BS_ENUM)
#undef BARRIER_SET_DECLARE_BS_ENUM
    UnknownBS
  };

  

//protected:
  // Fake RTTI support.  For a derived class T to participate
  // - T must have a corresponding Name entry.
  // - GetName<T> must be specialized to return the corresponding Name
  //   entry.
  // - If T is a base class, the constructor must have a FakeRtti
  //   parameter and pass it up to its base class, with the tag set
  //   augmented with the corresponding Name entry.
  // - If T is a concrete class, the constructor must create a
  //   FakeRtti object whose tag set includes the corresponding Name
  //   entry, and pass it up to its base class.


private:

  BarrierSetAssembler* _barrier_set_assembler;


//public:
  // Metafunction mapping a class derived from BarrierSetRv to the
  // corresponding Name enum tag.
  //template<typename T> struct GetName;

  // Metafunction mapping a Name enum type to the corresponding
  // lass derived from BarrierSetRv.
  //template<BarrierSetRv::Name T> struct GetType;

  // Note: This is not presently the Name corresponding to the
  // concrete class of this object.


  // Test whether this object is of the type corresponding to bsn.


  // End of fake RTTI support.

protected:
  BarrierSetRv(BarrierSetAssembler* barrier_set_assembler) :
    _barrier_set_assembler(barrier_set_assembler){}
  ~BarrierSetRv() { }
  Name _kind;

  template <class BarrierSetAssemblerT>
  static BarrierSetAssembler* make_barrier_set_assembler() {
    return NOT_ZERO(new BarrierSetAssemblerT()) ZERO_ONLY(NULL);
  }


public:
  // Support for optimizing compilers to call the barrier set on slow path allocations
  // that did not enter a TLAB. Used for e.g. ReduceInitialCardMarks.
  // The allocation is safe to use iff it returns true. If not, the slow-path allocation
  // is redone until it succeeds. This can e.g. prevent allocations from the slow path
  // to be in old.
 // virtual void on_slowpath_allocation_exit(JavaThread* thread, oop new_obj) {}
  virtual void on_thread_create(Thread* thread) {}
  virtual void on_thread_destroy(Thread* thread) {}


public:
  // Print a description of the memory for the barrier set
  //virtual void print_on(outputStream* st) const = 0;

  static BarrierSetRv* barrier_set() { return _barrier_set; }
  static void set_barrier_set(BarrierSetRv* barrier_set);
  BarrierSetRv::Name kind() { return _kind; }

  BarrierSetAssembler* barrier_set_assembler() {
    assert(_barrier_set_assembler != NULL, "should be set");
    return _barrier_set_assembler;
  }



  // The AccessBarrier of a BarrierSetRv subclass is called by the Access API
  // (cf. oops/access.hpp) to perform decorated accesses. GC implementations
  // may override these default access operations by declaring an
  // AccessBarrier class in its BarrierSetRv. Its accessors will then be
  // automatically resolved at runtime.
  //
  // In order to register a new FooBarrierSet::AccessBarrier with the Access API,
  // the following steps should be taken:
  // 1) Provide an enum "name" for the BarrierSetRv in barrierSetConfig.hpp
  // 2) Make sure the barrier set headers are included from barrierSetConfig.inline.hpp
  // 3) Provide specializations for BarrierSetRv::GetName and BarrierSetRv::GetType.

};
template<typename T>
inline T* barrier_set_cast(BarrierSetRv* bs) {
  //assert(bs->is_a(BarrierSetRv::GetName<T>::value), "wrong type of barrier set");
  return static_cast<T*>(bs);
}


#endif // SHARE_VM_GC_SHARED_BARRIERSET_HPP
