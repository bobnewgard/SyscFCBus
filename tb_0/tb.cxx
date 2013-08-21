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

#include "tb.h"

using namespace SyscJson;

Checker::Checker(sc_module_name arg_nm, SyscFCBus::Bus_src<be> * arg_bs)
{
    this->msg   = unique_ptr<Msg>(new Msg(this->name()));
    this->bus   = arg_bs;
    this->pass  = true;
    this->count = 3;

    SC_CTHREAD(check, this->clk_i.neg());
}

Checker::~Checker(void) { }

void
Checker::set_count(unsigned arg)
{
    this->count = arg;
}

bool
Checker::get_pass(void)
{
    return this->pass;
}

void
Checker::check(void)
{
    Bus<be>  sig_bus       = bus_rst<be>();
    bool     sig_dav       = false;
    bool     sig_end       = false;
    unsigned pkt_cnt       = 0;
    unsigned byte_max      = (1 << be);
    unsigned exp_frame_len = 0;
    unsigned obs_frame_len = 0;
    unsigned acc_frame_len = 64;
    str_vec  exp_frame_bytes;
    str_vec  obs_frame_bytes;

    this->end_o = sig_end;

    while (true)
    {
        wait();

        sig_bus = this->bus_i;
        sig_dav = this->dav_i;

        SCDat<be>      scdat   = sig_bus.dat;
        unsigned       mod_cnt = bus_get_byte_cnt(sig_bus);

        if (sig_bus.eof && sig_bus.val && sig_dav)
        {
            pkt_cnt++;
        }

        if (sig_bus.sof && sig_bus.val && sig_dav)
        {
            obs_frame_len = mod_cnt;
            exp_frame_len = this->bus->get_cur_byte_cnt();
            exp_frame_bytes = this->bus->get_cur_byte_vec();
            obs_frame_bytes.clear();
        }
        else if (sig_bus.val && sig_dav)
        {
            obs_frame_len = obs_frame_len + mod_cnt;
        }

        if (sig_bus.val && sig_dav)
        {
            for (unsigned i = 0 ; i < mod_cnt; i++)
            {
                unsigned      byte_slice = byte_max - i;
                unsigned      msb        = (byte_slice * 8) - 1;
                unsigned      lsb        = (byte_slice * 8) - 8;
                unsigned      byte       = scdat.range(msb, lsb).to_uint();
                stringstream  ss;

                ss << hex << uppercase << setw(2) << setfill('0') << byte;

                obs_frame_bytes.push_back(ss.str());
            }
        }

        if (sig_bus.eof && sig_bus.val && sig_dav)
        {
            bool tmp_pass = true;
            
            if (false)
            {
                cerr << sig_bus << endl;
            }

            if (obs_frame_len != acc_frame_len)
            {
                tmp_pass = false;
                this->msg->report_inf
                (
                    "miscompare, accumulated frame_len is" + SP + to_string(acc_frame_len)
                    + ", observed frame_len is" + SP + to_string(obs_frame_len)
                    + ", FAIL"
                );
            }

            if (obs_frame_len != exp_frame_len)
            {
                tmp_pass = false;
                this->msg->report_inf
                (
                    "miscompare, expected frame_len is" + SP + to_string(exp_frame_len)
                    + ", observed frame_len is" + SP + to_string(obs_frame_len)
                    + ", FAIL"
                );
            }

            for (unsigned i = 0 ; i < obs_frame_len ; i++)
            {
                if (obs_frame_bytes[i] != exp_frame_bytes[i])
                {
                    tmp_pass = false;
                    this->msg->report_inf
                    (
                        "miscompare, expected byte at position" + SP + to_string(i) + SP + "is" + SP + exp_frame_bytes[i]
                        + ", observed byte at position" + to_string(i) + SP + "is" + SP + obs_frame_bytes[i]
                        + ", FAIL"
                    );

                    break;
                }
            }

            if (tmp_pass)
            {
                this->msg->report_inf("frame_len" + SP + to_string(exp_frame_len) + SP + "OK");
            }
            else
            {
                this->msg->report_inf("frame_len" + SP + to_string(exp_frame_len) + SP + "FAIL");

                string tmp_str = "miscompare, expected bytes are";

                for (unsigned i = 0 ; i < exp_frame_len ; i++)
                {
                    tmp_str = tmp_str + SP + exp_frame_bytes[i];
                }

                this->msg->report_inf(tmp_str);

                tmp_str = "miscompare, observed bytes are";

                for (unsigned i = 0 ; i < obs_frame_len ; i++)
                {
                    tmp_str = tmp_str + SP + obs_frame_bytes[i];
                }

                this->msg->report_inf(tmp_str);
            }

            this->pass    = this->pass & tmp_pass;
            acc_frame_len = acc_frame_len + 1;
        }

        sig_end = (pkt_cnt >= this->count);

        if (sig_end)
        {
            this->msg->report_inf("packet count met; stopping");
        }

        this->end_o = sig_end;
    }
}


ReqMux::ReqMux(sc_module_name arg_nm)
{
    SC_METHOD(run);
    sensitive << b_i << a_i << s_i;
}

ReqMux::~ReqMux(void) { }

void
ReqMux::run(void)
{
    bool sig_s = this->s_i;
    bool sig_a = this->a_i;
    bool sig_b = this->b_i;

    if (sig_s)
    {
        this->y_o = sig_b;
    }
    else
    {
        this->y_o = sig_a;
    }
}

ReqDly::ReqDly(sc_module_name arg_nm, unsigned arg_dl)
{
    if (arg_dl > 3U)
    {
        this->delay = 3U;
    }
    else if (arg_dl < 1U)
    {
        this->delay = 1U;
    }
    else
    {
        this->delay = arg_dl;
    }

    SC_CTHREAD(run, this->clk_i.pos());
}

ReqDly::~ReqDly(void) { }

void
ReqDly::run(void)
{
    bool     sig_z1 = false;
    bool     sig_z2 = false;
    bool     sig_z3 = false;

    this->req_o  = false;

    while (true)
    {
        wait();

        sig_z3 = sig_z2;
        sig_z2 = sig_z1;
        sig_z1 = this->req_i;

        switch (this->delay)
        {
            case 1U: { this->req_o = sig_z1 ; break; }
            case 2U: { this->req_o = sig_z2 ; break; }
            case 3U: { this->req_o = sig_z3 ; break; }
        }
    }
}

tb::tb(sc_module_name arg_nm, unsigned arg_dly)
{
    this->req_delay   = arg_dly;
    this->clk_freq_hz = 156.250e6;
    this->drv_path    = "./pydrv_server.py";
    this->drv_handler = "dot3_incr_len";
    this->drv_request = "{}";
    this->msg         = unique_ptr<Msg>(new Msg(this->name()));
    this->drv         = new DrvClient(this->drv_path);
    this->i_clk       = new Clk<bool>("i_clk", this->clk_freq_hz, 0.5, 1.0, SC_NS, true);
    this->i_bus       = new Bus_src<be>("i_bus", this->drv, this->drv_handler, this->drv_request);
    this->i_dly       = new ReqDly("i_dly", this->req_delay);
    this->i_mux       = new ReqMux("i_mux");
    this->i_chk       = new Checker("i_chk", this->i_bus);

    this->msg->report_inf("datapath is" + SP + to_string((1 << be) * 8) + SP + "bits");
    this->msg->report_inf("req_delay is" + SP + to_string(this->req_delay));

    this->tb_clk      = true;
    this->tb_dav      = true;
    this->tb_sel      = (req_delay > 0);

    this->i_clk->clk_o ( tb_clk  );

    this->i_bus->bus_o ( bus_bus );
    this->i_bus->sav_o ( bus_sav );
    this->i_bus->cnt_o ( bus_cnt );
    this->i_bus->req_o ( bus_req );
    this->i_bus->dav_i ( tb_dav  );
    this->i_bus->clk_i ( tb_clk  );
    this->i_bus->ack_i ( mux_req );

    this->i_dly->req_o ( dly_req );
    this->i_dly->req_i ( bus_req );
    this->i_dly->clk_i ( tb_clk  );

    this->i_mux->y_o   ( mux_req );
    this->i_mux->b_i   ( dly_req );
    this->i_mux->a_i   ( bus_req );
    this->i_mux->s_i   ( tb_sel  );

    this->i_chk->end_o ( chk_end );
    this->i_chk->bus_i ( bus_bus );
    this->i_chk->dav_i ( tb_dav  );
    this->i_chk->clk_i ( tb_clk  );
}

tb::~tb(void)
{
    delete this->i_mux;
    delete this->i_dly;
    delete this->i_bus;
    delete this->i_clk;
    delete this->drv;
}
