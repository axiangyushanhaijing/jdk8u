/*
 * Copyright (c) 2000, 2012, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_VM_MEMORY_BARRIERSET_HPP
#define SHARE_VM_MEMORY_BARRIERSET_HPP
#include "runtime/jniHandles.hpp"
#include "runtime/thread.hpp"
#include "memory/memRegion.hpp"
#include "oops/oopsHierarchy.hpp"
#include "asm/macroAssembler.hpp"
//#include "memory/universe.hpp"
#include "gc_interface/collectedHeap.hpp"

#define __ masm->
//#include "temp_riscv64.hpp"
class BarrierSetAssembler;

// This class provides the interface between a barrier implementation and
// the rest of the system.

class BarrierSet: public CHeapObj<mtGC> {
  friend class VMStructs;
  static BarrierSet* _barrier_set;
public:
  enum Name {
    ModRef,
    CardTableModRef,
    CardTableExtension,
    G1SATBCT,
    G1SATBCTLogging,
    Other,
    Uninit
  };

  enum Flags {
    None                = 0,
    TargetUninitialized = 1
  };
protected:
  int _max_covered_regions;
  Name _kind;

public:

  BarrierSet() { _kind = Uninit; }
  // To get around prohibition on RTTI.
  BarrierSet::Name kind() { return _kind; }
  virtual bool is_a(BarrierSet::Name bsn) = 0;

  // These operations indicate what kind of barriers the BarrierSet has.
  virtual bool has_read_ref_barrier() = 0;
  virtual bool has_read_prim_barrier() = 0;
  virtual bool has_write_ref_barrier() = 0;
  virtual bool has_write_ref_pre_barrier() = 0;
  virtual bool has_write_prim_barrier() = 0;
  static BarrierSet* barrier_set() { return _barrier_set; }

  // These functions indicate whether a particular access of the given
  // kinds requires a barrier.
  virtual bool read_ref_needs_barrier(void* field) = 0;
  virtual bool read_prim_needs_barrier(HeapWord* field, size_t bytes) = 0;
  virtual bool write_prim_needs_barrier(HeapWord* field, size_t bytes,
                                        juint val1, juint val2) = 0;

  // The first four operations provide a direct implementation of the
  // barrier set.  An interpreter loop, for example, could call these
  // directly, as appropriate.

  // Invoke the barrier, if any, necessary when reading the given ref field.
  virtual void read_ref_field(void* field) = 0;

  // Invoke the barrier, if any, necessary when reading the given primitive
  // "field" of "bytes" bytes in "obj".
  virtual void read_prim_field(HeapWord* field, size_t bytes) = 0;

  // Invoke the barrier, if any, necessary when writing "new_val" into the
  // ref field at "offset" in "obj".
  // (For efficiency reasons, this operation is specialized for certain
  // barrier types.  Semantically, it should be thought of as a call to the
  // virtual "_work" function below, which must implement the barrier.)
  // First the pre-write versions...
  template <class T> inline void write_ref_field_pre(T* field, oop new_val);
private:
 BarrierSetAssembler* _barrier_set_assembler;
  // Keep this private so as to catch violations at build time.
  virtual void write_ref_field_pre_work(     void* field, oop new_val) { guarantee(false, "Not needed"); };
protected:
  virtual void write_ref_field_pre_work(      oop* field, oop new_val) {};
  virtual void write_ref_field_pre_work(narrowOop* field, oop new_val) {};
public:

  // ...then the post-write version.
  inline void write_ref_field(void* field, oop new_val, bool release = false);
protected:
  virtual void write_ref_field_work(void* field, oop new_val, bool release = false) = 0;
public:

  // Invoke the barrier, if any, necessary when writing the "bytes"-byte
  // value(s) "val1" (and "val2") into the primitive "field".
  virtual void write_prim_field(HeapWord* field, size_t bytes,
                                juint val1, juint val2) = 0;
  BarrierSetAssembler* barrier_set_assembler() {
    assert(_barrier_set_assembler != NULL, "should be set");
    return _barrier_set_assembler;
  }

  // Operations on arrays, or general regions (e.g., for "clone") may be
  // optimized by some barriers.

  // The first six operations tell whether such an optimization exists for
  // the particular barrier.
  virtual bool has_read_ref_array_opt() = 0;
  virtual bool has_read_prim_array_opt() = 0;
  virtual bool has_write_ref_array_pre_opt() { return true; }
  virtual bool has_write_ref_array_opt() = 0;
  virtual bool has_write_prim_array_opt() = 0;

  virtual bool has_read_region_opt() = 0;
  virtual bool has_write_region_opt() = 0;

  // These operations should assert false unless the correponding operation
  // above returns true.  Otherwise, they should perform an appropriate
  // barrier for an array whose elements are all in the given memory region.
  virtual void read_ref_array(MemRegion mr) = 0;
  virtual void read_prim_array(MemRegion mr) = 0;

  // Below length is the # array elements being written
  virtual void write_ref_array_pre(oop* dst, int length,
                                   bool dest_uninitialized = false) {}
  virtual void write_ref_array_pre(narrowOop* dst, int length,
                                   bool dest_uninitialized = false) {}
  // Below count is the # array elements being written, starting
  // at the address "start", which may not necessarily be HeapWord-aligned
  inline void write_ref_array(HeapWord* start, size_t count);

  // Static versions, suitable for calling from generated code;
  // count is # array elements being written, starting with "start",
  // which may not necessarily be HeapWord-aligned.
  static void static_write_ref_array_pre(HeapWord* start, size_t count);
  static void static_write_ref_array_post(HeapWord* start, size_t count);

protected:
  virtual void write_ref_array_work(MemRegion mr) = 0;
public:
  virtual void write_prim_array(MemRegion mr) = 0;

  virtual void read_region(MemRegion mr) = 0;

  // (For efficiency reasons, this operation is specialized for certain
  // barrier types.  Semantically, it should be thought of as a call to the
  // virtual "_work" function below, which must implement the barrier.)
  inline void write_region(MemRegion mr);
protected:
  virtual void write_region_work(MemRegion mr) = 0;
public:

  // Some barrier sets create tables whose elements correspond to parts of
  // the heap; the CardTableModRefBS is an example.  Such barrier sets will
  // normally reserve space for such tables, and commit parts of the table
  // "covering" parts of the heap that are committed.  The constructor is
  // passed the maximum number of independently committable subregions to
  // be covered, and the "resize_covoered_region" function allows the
  // sub-parts of the heap to inform the barrier set of changes of their
  // sizes.
  BarrierSet(int max_covered_regions) :
    _max_covered_regions(max_covered_regions) {}

  // Inform the BarrierSet that the the covered heap region that starts
  // with "base" has been changed to have the given size (possibly from 0,
  // for initialization.)
  virtual void resize_covered_region(MemRegion new_region) = 0;

  // If the barrier set imposes any alignment restrictions on boundaries
  // within the heap, this function tells whether they are met.
  virtual bool is_aligned(HeapWord* addr) = 0;

  // Print a description of the memory for the barrier set
  virtual void print_on(outputStream* st) const = 0;
};

class BarrierSetAssembler: public CHeapObj<mtGC> {
private:
  void incr_allocated_bytes(MacroAssembler* masm,
                            Register var_size_in_bytes, int con_size_in_bytes,
                            Register t1 = noreg);

public:
  virtual void load_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                       Register dst, Address src, Register tmp1, Register tmp_thread);
  virtual void store_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                        Address dst, Register val, Register tmp1, Register tmp2);
  virtual void obj_equals(MacroAssembler* masm, Register obj1, Register obj2, Label& equal, bool is_far = false);
  virtual void obj_nequals(MacroAssembler* masm, Register obj1, Register obj2, Label& nequal, bool is_far = false);
  virtual void try_resolve_jobject_in_native(MacroAssembler* masm, Register jni_env,
                                             Register obj, Register tmp, Label& slowpath);
  virtual void tlab_allocate(MacroAssembler* masm,
    Register obj,                      // result: pointer to object after successful allocation
    Register var_size_in_bytes,        // object size in bytes if unknown at compile time; invalid otherwise
    int      con_size_in_bytes,        // object size in bytes if   known at compile time
    Register tmp1,                     // temp register
    Register tmp2,                     // temp register
    Label&   slow_case,                // continuation point if fast allocation fails
    bool is_far = false                // the distance of label slowcase could be more than 12KiB in C1
  );
  void eden_allocate(MacroAssembler* masm,
    Register obj,                      // result: pointer to object after successful allocation
    Register var_size_in_bytes,        // object size in bytes if unknown at compile time; invalid otherwise
    int      con_size_in_bytes,        // object size in bytes if   known at compile time
    Register tmp1,                     // temp register
    Label&   slow_case,                // continuation point if fast allocation fails
    bool is_far = false                // the distance of label slowcase could be more than 12KiB in C1
  );


};

void BarrierSetAssembler::load_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                                  Register dst, Address src, Register tmp1, Register tmp_thread) {
  assert_cond(masm != NULL);

  // LR is live. It must be saved around calls.

  bool in_heap = (decorators & IN_HEAP) != 0;
  bool in_native = (decorators & IN_NATIVE) != 0;
  bool is_not_null = (decorators & IS_NOT_NULL) != 0;
  switch (type) {
  case T_OBJECT:
  case T_ARRAY: {
    if (in_heap) {
      if (UseCompressedOops) {
        __ lwu(dst, src);
        if (is_not_null) {
          __ decode_heap_oop_not_null(dst);
        } else {
          __ decode_heap_oop(dst);
        }
      } else {
        __ ld(dst, src);
      }
    } else {
      assert(in_native, "why else?");
      __ ld(dst, src);
    }
    break;
  }
  case T_BOOLEAN: __ load_unsigned_byte (dst, src); break;
  case T_BYTE:    __ load_signed_byte   (dst, src); break;
  case T_CHAR:    __ load_unsigned_short(dst, src); break;
  case T_SHORT:   __ load_signed_short  (dst, src); break;
  case T_INT:     __ lw                 (dst, src); break;
  case T_LONG:    __ ld                 (dst, src); break;
  case T_ADDRESS: __ ld                 (dst, src); break;
  case T_FLOAT:   __ flw                (f10, src); break;
  case T_DOUBLE:  __ fld                (f10, src); break;
  default: Unimplemented();
  }
}

void BarrierSetAssembler::store_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                                   Address dst, Register val, Register tmp1, Register tmp2) {
  assert_cond(masm != NULL);
  bool in_heap = (decorators & IN_HEAP) != 0;
  bool in_native = (decorators & IN_NATIVE) != 0;
  switch (type) {
  case T_OBJECT:
  case T_ARRAY: {
    val = val == noreg ? zr : val;
    if (in_heap) {
      if (UseCompressedOops) {
        assert(!dst.uses(val), "not enough registers");
        if (val != zr) {
          __ encode_heap_oop(val);
        }
        __ sw(val, dst);
      } else {
        __ sd(val, dst);
      }
    } else {
      assert(in_native, "why else?");
      __ sd(val, dst);
    }
    break;
  }
  case T_BOOLEAN:
    __ andi(val, val, 0x1);  // boolean is true if LSB is 1
    __ sb(val, dst);
    break;
  case T_BYTE:    __ sb(val, dst); break;
  case T_CHAR:    __ sh(val, dst); break;
  case T_SHORT:   __ sh(val, dst); break;
  case T_INT:     __ sw(val, dst); break;
  case T_LONG:    __ sd(val, dst); break;
  case T_ADDRESS: __ sd(val, dst); break;
  case T_FLOAT:   __ fsw(f10,  dst); break;
  case T_DOUBLE:  __ fsd(f10,  dst); break;
  default: Unimplemented();
  }

}

void BarrierSetAssembler::obj_equals(MacroAssembler* masm, Register obj1, Register obj2, Label& equal, bool is_far) {
  assert_cond(masm != NULL);
  __ beq(obj1, obj2, equal, is_far);
}

void BarrierSetAssembler::obj_nequals(MacroAssembler* masm, Register obj1, Register obj2, Label& nequal, bool is_far) {
  assert_cond(masm != NULL);
  __ bne(obj1, obj2, nequal, is_far);
}

void BarrierSetAssembler::try_resolve_jobject_in_native(MacroAssembler* masm, Register jni_env,
                                                        Register obj, Register tmp, Label& slowpath) {
  assert_cond(masm != NULL);
  // If mask changes we need to ensure that the inverse is still encodable as an immediate
  STATIC_ASSERT(JNIHandles::weak_tag_mask == 1);
  __ andi(obj, obj, ~JNIHandles::weak_tag_mask);
  __ lw(obj, Address(obj, 0));             // *obj
}


void BarrierSetAssembler::incr_allocated_bytes(MacroAssembler* masm,
                                               Register var_size_in_bytes,
                                               int con_size_in_bytes,
                                               Register tmp1) {
  assert_cond(masm != NULL);
  assert(tmp1->is_valid(), "need temp reg");

  __ lw(tmp1, Address(xthread, in_bytes(JavaThread::allocated_bytes_offset())));
  if (var_size_in_bytes->is_valid()) {
    __ add(tmp1, tmp1, var_size_in_bytes);
  } else {
    __ add(tmp1, tmp1, con_size_in_bytes);
  }
  __ sw(tmp1, Address(xthread, in_bytes(JavaThread::allocated_bytes_offset())));
}
void BarrierSetAssembler::eden_allocate(MacroAssembler* masm, Register obj,
                                        Register var_size_in_bytes,
                                        int con_size_in_bytes,
                                        Register tmp1,
                                        Label& slow_case,
                                        bool is_far) {
  assert_cond(masm != NULL);
  assert_different_registers(obj, var_size_in_bytes, tmp1);
  if (!Universe::heap()->supports_inline_contig_alloc()) {
    __ j(slow_case);
  } else {
    Register end = tmp1;
    Label retry;
    int32_t offset = 0;
    __ bind(retry);

    // Get the current top of the heap
    ExternalAddress address_top((address) Universe::heap()->top_addr());
    __ la_patchable(t2, address_top, offset);
    __ addi(t2, t2, offset);
    __ lr_w(obj, t2, Assembler::aqrl);

    // Adjust it my the size of our new object
    if (var_size_in_bytes == noreg) {
      __ la(end, Address(obj, con_size_in_bytes));
    } else {
      __ add(end, obj, var_size_in_bytes);
    }

    // if end < obj then we wrapped around high memory
    __ bltu(end, obj, slow_case, is_far);

    Register heap_end = t1;
    // Get the current end of the heap
    ExternalAddress address_end((address) Universe::heap()->end_addr());
    offset = 0;
    __ la_patchable(heap_end, address_end, offset);
    __ lw(heap_end, Address(heap_end, offset));

    __ bgtu(end, heap_end, slow_case, is_far);

    // If heap_top hasn't been changed by some other thread, update it.
    __ sc_w(t1, end, t2, Assembler::rl);
    __ bnez(t1, retry);
    incr_allocated_bytes(masm, var_size_in_bytes, con_size_in_bytes, tmp1);
  }
}
#endif // SHARE_VM_MEMORY_BARRIERSET_HPP
