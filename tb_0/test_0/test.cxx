/*
 * Copyright 2013 Robert Newgard
 *
 * This file is part of SyscFCBus.
 *
 * SyscFCBus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SyscFCBus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SyscFCBus.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "test.h"

test::test(sc_module_name nm, unsigned dl) : tb(nm, dl)
{
    this->tf = sc_create_vcd_trace_file("test");

    sc_trace(this->tf, this->tb_clk,               "tb_clk"              );
    sc_trace(this->tf, this->tb_dav,               "tb_dav"              );
    sc_trace(this->tf, this->tb_sel,               "tb_sel"              );
    sc_trace(this->tf, this->bus_cnt,              "bus_cnt"             );
    sc_trace(this->tf, this->bus_req,              "bus_req"             );
    sc_trace(this->tf, this->bus_bus,              "bus_bus"             );
    sc_trace(this->tf, this->i_bus->drv_s,         "i_bus.drv_s"         );
    sc_trace(this->tf, this->i_bus->drv_c,         "i_bus.drv_c"         );
    sc_trace(this->tf, this->i_bus->drv_lc,        "i_bus.drv_lc"        );
    sc_trace(this->tf, this->i_bus->drv_ln,        "i_bus.drv_ln"        );
    sc_trace(this->tf, this->dly_req,              "dly_req"             );
    sc_trace(this->tf, this->mux_req,              "mux_req"             );
    sc_trace(this->tf, this->chk_end,              "chk_end"             );
    sc_trace(this->tf, this->i_chk->bus_i,         "i_chk.bus_i"         );

    SC_THREAD(test_timeout);
    SC_THREAD(test_execute);
        sensitive << this->chk_end;
    // set_stack_size(100000);
    // OSCI 5.2.16, sc_core::sc_module::set_stack_size(size_t);
}

test::~test(void)
{
    sc_close_vcd_trace_file(this->tf);
}

void
test::test_execute(void)
{
    wait();

    if (this->i_chk->get_pass())
    {
        SC_REPORT_INFO(this->name(), "PASS");
    }
    else
    {
        SC_REPORT_ERROR(this->name(), "FAIL");
    }

    sc_stop();
}
void

test::test_timeout(void)
{
    wait(20, SC_MS);
    SC_REPORT_FATAL(this->name(), "timeout, simulation ran for 20 ms");
}
