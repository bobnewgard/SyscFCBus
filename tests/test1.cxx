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

#include <SyscFCBus.h>

using namespace std;
using namespace SyscFCBus;
using namespace SyscMsg;
using namespace SyscMsg::Chars;

bool enable_test_00 = true;
bool enable_test_01 = true;
bool enable_test_02 = true;
bool enable_test_03 = true;
bool enable_test_04 = true;
bool enable_test_05 = true;
bool enable_test_06 = true;
bool enable_test_07 = true;
bool enable_test_08 = true;

template <unsigned T_be>
bool test_operator_ostream(Msg& msg, Bus<T_be>& arg_bus)
{
    msg.cerr_inf("testing ostream");
    cerr << arg_bus;
    return true;
}

template <unsigned T_be>
bool test_operator_copy(Msg& msg, Bus<T_be>& arg_bus, Dat<T_be>& arg_dat)
{
    Bus<T_be> cmp_bus = arg_bus;
    string    test    = "testing that = creates copy of Bus:";

    arg_bus.dat   = arg_dat;
    cmp_bus.dat   = ~arg_dat;

    if (arg_bus == cmp_bus)
    {
        msg.cerr_err(test + SP + "FAIL");
        return false;
    }
    else
    {
        msg.cerr_inf(test + SP + "OK");
        return true;
    }
}

void
test_message(Msg& msg, unsigned arg)
{
    cerr << NL;
    msg.cerr_inf("be is" + SP + to_string(arg));
}

int
sc_main(int argc, char *argv[])
{
    bool              pass = true;
    Msg               msg("test1:");

    msg.cerr_inf("start");

    if (enable_test_00)
    {
        constexpr unsigned be  = 0;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat = 0xA5;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_01)
    {
        constexpr unsigned be  = 1;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat = 0xA5A5;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_02)
    {
        constexpr unsigned be  = 2;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat = 0xA5A5A5A5;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_03)
    {
        constexpr unsigned be  = 3;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat = 0xA5A5A5A5A5A5A5A5;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_04)
    {
        constexpr unsigned be = 4;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat;

        dat.range( 63, 0) = 0xA5A5A5A5A5A5A5A5;
        dat.range(127,64) = 0xB6B6B6B6B6B6B6B6;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_05)
    {
        constexpr unsigned be  = 5;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat;

        dat.range( 63, 0)  = 0xA5A5A5A5A5A5A5A5;
        dat.range(127,64)  = 0xB6B6B6B6B6B6B6B6;
        dat.range(191,128) = 0xC7C7C7C7C7C7C7C7;
        dat.range(255,192) = 0xD8D8D8D8D8D8D8D8;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_06)
    {
        constexpr unsigned be  = 6;
        Bus<be>            bus = bus_rst<be>();
        Dat<be>            dat;

        dat.range( 63, 0)  = 0xA5A5A5A5A5A5A5A5;
        dat.range(127,64)  = 0xB6B6B6B6B6B6B6B6;
        dat.range(191,128) = 0xC7C7C7C7C7C7C7C7;
        dat.range(255,192) = 0xD8D8D8D8D8D8D8D8;
        dat.range(319,256) = 0xE9E9E9E9E9E9E9E9;
        dat.range(383,320) = 0xFAFAFAFAFAFAFAFA;
        dat.range(447,384) = 0x0B0B0B0B0B0B0B0B;
        dat.range(511,448) = 0x1C1C1C1C1C1C1C1C;

        test_message(msg, be);

        if (! test_operator_copy<be>(msg, bus, dat)) { pass = false; }
        if (! test_operator_ostream<be>(msg, bus))   { pass = false; }
    }

    if (enable_test_07)
    {
        string     drvh = "handler";
        string     drvr = "{}";
        Bus_src<0> src("i_src_0", NULL, drvh, drvr);

        cerr << NL;
        msg.cerr_inf("instatiating Bus_src<0>: OK");
    }

    if (enable_test_08)
    {
        string     drvh = "handler";
        string     drvr = "{}";
        Bus_src<4> src("i_src_4", NULL, drvh, drvr);

        cerr << NL;
        msg.cerr_inf("instatiating Bus_src<4>: OK");
    }

    cerr << NL;

    if (pass)
    {
        msg.cerr_inf("pass");
    }
    else
    {
        msg.cerr_err("fail");
    }

    return 0;
}
