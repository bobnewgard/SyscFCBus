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

// tb.h

#ifndef _TB_H_
    #define _TB_H_

    #include <systemc>
    #include <SyscClk.h>
    #include <SyscDrv.h>
    #include <SyscFCBus.h>
    #include "cfg_be.h"

    using namespace std;
    using namespace sc_core;
    using namespace sc_dt;
    using namespace SyscClk;
    using namespace SyscFCBus;
    using namespace SyscDrv;
    using namespace SyscMsg;
    using namespace SyscMsg::Chars;

    class Checker : public sc_module
    {
        private:
            unique_ptr<Msg>      msg;
            Bus_src<be>        * bus;
            bool                 pass;
            unsigned             count;

        public:
            SC_HAS_PROCESS(Checker);
            Checker(sc_module_name, SyscFCBus::Bus_src<be>*);
            ~Checker(void);

            sc_out <bool>     end_o;
            sc_in  <Bus<be>>  bus_i;
            sc_in  <bool>     dav_i;
            sc_in  <bool>     clk_i;

            void check(void);
            void set_count(unsigned);
            bool get_pass(void);
    };

    class ReqMux : public sc_module
    {
        public:
            SC_HAS_PROCESS(ReqMux);
            ReqMux(sc_module_name);
            ~ReqMux(void);

            sc_out   <bool>     y_o;
            sc_in    <bool>     b_i;
            sc_in    <bool>     a_i;
            sc_in    <bool>     s_i;

            void run(void);
    };

    class ReqDly : public sc_module
    {
        private:
            unsigned delay;

        public:
            SC_HAS_PROCESS(ReqDly);
            ReqDly(sc_module_name, unsigned);
            ~ReqDly(void);

            sc_out   <bool>     req_o;
            sc_in    <bool>     req_i;
            sc_in    <bool>     clk_i;

            void run(void);
    };

    class tb : public sc_module
    {
        private:
            double            clk_freq_hz;
            string            drv_path;
            string            drv_handler;
            string            drv_request;
            unsigned          req_delay;
            unique_ptr<Msg>   msg;
            DrvClient       * drv;

        public:
            SC_HAS_PROCESS(tb);
            tb(sc_module_name, unsigned);
            ~tb(void);

            Clk<bool>   * i_clk;
            Bus_src<be> * i_bus;
            ReqDly      * i_dly;
            ReqMux      * i_mux;
            Checker     * i_chk;

            sc_signal <bool    > tb_clk;
            sc_signal <bool    > tb_dav;
            sc_signal <bool    > tb_sel;
            sc_signal <bool    > bus_req;
            sc_signal <uint32_t> bus_cnt;
            sc_signal <Bus<be> > bus_bus;
            sc_signal <bool    > bus_sav;
            sc_signal <bool    > dly_req;
            sc_signal <bool    > mux_req;
            sc_signal <bool    > chk_end;
    };
#endif
