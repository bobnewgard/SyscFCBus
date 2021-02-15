/*
 * Copyright 2013-2021 Robert Newgard
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
#include "cfg_test.h"

int sc_main(int argc, char **argv)
{
    test test_0(test_frm_nam, test_req_dly);
    test_0.i_chk->set_count(test_frm_cnt);
    sc_start();
    return 0;
}
