/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"

using namespace cpu::ppu;

PPUThread::PPUThread(u32 entry)
{
    m_translator = new Interpreter(*this);

    // Initialize stack of size 0x10000
    m_stackAddr = nucleus.memory(SEG_STACK).alloc(0x10000, 0x100);
    m_stackPointer = m_stackAddr + 0x10000;

    const u32 entry_pc = nucleus.memory.read32(entry);
    const u32 entry_rtoc = nucleus.memory.read32(entry+4);

    // Initialize Program Counter
    pc = entry_pc;

    // Initialize UISA Registers (TODO: All of this might be wrong)
    gpr[0] = entry_pc;
    gpr[1] = m_stackPointer - 0x200;
    gpr[2] = entry_rtoc;
    gpr[3] = 0;
    gpr[4] = m_stackPointer - 0x80;
    gpr[5] = gpr[4] + 0x10;
    gpr[11] = entry;
    gpr[12] = nucleus.lv2.proc.param.malloc_pagesize;
    gpr[13] = nucleus.memory(SEG_USER_MEMORY).getBaseAddr() + 0x7060; // TLS
    cr.CR = 0x22000082;
    tbl = 1;
    tbu = 1;

    // Arguments passed to sys_initialize_tls on liblv2.sprx's start function
    gpr[7] = 0x0; // TODO
    gpr[8] = 0x0; // TODO
    gpr[9] = 0x0; // TODO
    gpr[10] = 0x90;
}

PPUThread::~PPUThread()
{
    // Destroy stack
    nucleus.memory(SEG_STACK).free(m_stackAddr);
}
