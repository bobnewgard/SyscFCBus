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

/** \file  SyscFCBus.h
 *  \brief Declares the Bus structure, it's helpers, and the Bus_src class.
 *
 *  This file declares the Bus structure along with it's helper types,
 *  functions and constants and the Bus_src class.
 *
 */

#ifndef _SYSCFCBUS_H_
    #define _SYSCFCBUS_H_

    #include <iostream>
    #include <iomanip>
    #include <type_traits>
    #include <systemc>
    #include <SyscMsg.h>
    #include <SyscDrv.h>
    #include <SyscJson.h>

    /** \brief Namespace for the SyscFCBus templates
     *
     */

    namespace SyscFCBus
    {
        using namespace std;

        /** \class no_connect
         *  \brief Value assigned to mod element for 8-bit Bus instances.
         */

        class no_connect {};

        inline bool operator==(const no_connect& l, const no_connect & r)
        {
            return true;
        }

        inline bool operator!=(const no_connect& l, const no_connect & r)
        {
            return false;
        }

        inline ostream& operator<<(ostream& os, const no_connect & arg)
        {
            os << "null";
            return os;
        }

        inline void sc_trace(sc_trace_file * tf, const no_connect & arg, const std::string & nm)
        {
            sc_trace(tf, false, nm);
        }

        /** \struct typ_mod
         *  \brief Type definition for Bus.mod
         */

        template <unsigned T_be> struct typ_mod     { using typ = uint32_t;   };
        template <>              struct typ_mod<1U> { using typ = bool;       };
        template <>              struct typ_mod<0U> { using typ = no_connect; };

        /** \struct typ_dat
         *  \brief Type definition for Bus.dat
         */

        template <unsigned T_be> struct typ_dat     { using typ = uint32_t;          };
        template <>              struct typ_dat<3U> { using typ = uint64_t;          };
        template <>              struct typ_dat<4U> { using typ = sc_dt::sc_bv<128>; };
        template <>              struct typ_dat<5U> { using typ = sc_dt::sc_bv<256>; };
        template <>              struct typ_dat<6U> { using typ = sc_dt::sc_bv<512>; };

        /** \struct typ_scdat
         *  \brief  Type definition for manipulating Bus.dat using sc_dt::sc_uint or sc_dt::sc_bv semantics
         */

        template <unsigned T_be> struct typ_scdat     { using typ = sc_dt::sc_uint<32>; };
        template <>              struct typ_scdat<3U> { using typ = sc_dt::sc_uint<64>; };
        template <>              struct typ_scdat<4U> { using typ = sc_dt::sc_bv<128>;  };
        template <>              struct typ_scdat<5U> { using typ = sc_dt::sc_bv<256>;  };
        template <>              struct typ_scdat<6U> { using typ = sc_dt::sc_bv<512>;  };

        template <unsigned T_be> using Mod   = typename typ_mod<T_be>::typ;
        template <unsigned T_be> using Dat   = typename typ_dat<T_be>::typ;
        template <unsigned T_be> using SCDat = typename typ_scdat<T_be>::typ;

        /** \struct Bus
         *  \brief  Representation of a frame or cell as carried on a datapath.
         *
         *  <h2 class="mp">Template Parameter</h2>
         *
         *  %Bus width is specified by T_be.
         *  The "be" in T_be stands for "byte exponent" and the number of bytes in
         *  the bus is 2^T_be.
         *
         *  <h2 class="mp">%Bus Fields</h2>
         *
         *  %Bus fields are
         *  + usr[31:0]
         *  + err
         *  + val
         *  + sof
         *  + eof
         *  + dat[((M*8)-1):0]
         *  + mod[(N-1):0]
         *
         *  Where N is the byte exponent and M is 2^N.
         *
         *  <h3 class="mp">Mod Field</h3>
         *
         *  When T_be = 0, mod is instantiated as the empty class SyscFCBus::no_connect.
         */
        /** \fn    Bus::static_assert()
         *  \brief Ensures failure at compile time for unsupported values of T_be
         */
        /** \fn    Bus::operator=()
         *  \brief Copy constructor for Bus instances
         */
        /** \var   Bus::usr
         *  \brief Unspecified 32-bit field for application use.  May be used for
         *  the ATM cell fields {usr[3:0], vpi[11:0], vci[15:0]}.
         */
        /** \var   Bus::err
         *  \brief Indicates that the frame has an error in this or a previous clock
         */
        /** \var   Bus::val
         *  \brief Indicates that the data in the Bus structure is valid
         */
        /** \var   Bus::sof
         *  \brief Indicates that the Bus structure is the first in the frame
         */
        /** \var   Bus::eof
         *  \brief Indicates that the Bus structure is the last in the frame
         */
        /** \var   Bus::mod
         *  \brief Indicates the number of valid bytes
         */
        /** \var   Bus::dat
         *  \brief For the datapath bytes
         */

        template <unsigned T_be>
        struct Bus
        {
            static_assert((T_be < 7), "Bus byte count out of range");

            uint32_t  usr;
            bool      err;
            bool      val;
            bool      sof;
            bool      eof;
            Mod<T_be> mod;
            Dat<T_be> dat;

            static constexpr uint32_t bits_usr = 31;
            static constexpr uint32_t bits_mod = T_be;
            static constexpr uint32_t bits_dat = 8 * (1 << T_be);

            Bus<T_be>& operator=(const Bus<T_be>& arg)
            {
                if (this == &arg)
                {
                    return *this;
                }

                usr = arg.usr;
                err = arg.err;
                val = arg.val;
                sof = arg.sof;
                eof = arg.eof;
                mod = arg.mod;
                dat = arg.dat;

                return *this;
            }
        };

        template <unsigned T_be>
        typename std::enable_if<(T_be == 0U), Bus<T_be>>::type bus_rst(void)
        {
            return {0,false,false,false,false,{},0};
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 1U), Bus<T_be>>::type bus_rst(void)
        {
            return {0,false,false,false,false,false,0};
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be > 1U), Bus<T_be>>::type bus_rst(void)
        {
            return {0,false,false,false,false,0,0};
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 0U), Mod<T_be>>::type mod_rst(void)
        {
            return {};
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 1U), Mod<T_be>>::type mod_rst(void)
        {
            return false;
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be > 1U), Mod<T_be>>::type mod_rst(void)
        {
            return 0;
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 0U), Mod<T_be>>::type mod_set(uint32_t arg)
        {
            return {};
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 1U), Mod<T_be>>::type mod_set(uint32_t arg)
        {
            return (arg != 0);
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be > 1U), Mod<T_be>>::type mod_set(uint32_t arg)
        {
            return arg;
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be == 0U), unsigned>::type mod_get_uint(Mod<T_be> arg)
        {
            return 0;
        }

        template <unsigned T_be>
        typename std::enable_if<(T_be > 0U), unsigned>::type mod_get_uint(Mod<T_be> arg)
        {
            return arg;
        }

        template <unsigned T_be>
        unsigned bus_get_byte_cnt(Bus<T_be> arg)
        {
            unsigned ret = 0;
            unsigned mod = 0;
            unsigned max = (1 << T_be);

            if (T_be > 0)
            {
                mod = mod_get_uint<T_be>(arg.mod);
            }

            if (mod == 0)
            {
                 ret = max;
            }
            else
            {
                ret = mod;
            }

            return ret;
        }

        template <unsigned T_be>
        inline bool operator==(const Bus<T_be>& l, const Bus<T_be>& r)
        {
            if (l.usr != r.usr) { return false; }
            if (l.err != r.err) { return false; }
            if (l.val != r.val) { return false; }
            if (l.sof != r.sof) { return false; }
            if (l.eof != r.eof) { return false; }
            if (l.mod != r.mod) { return false; }
            if (l.dat != r.dat) { return false; }
            return true;
        }

        template <unsigned T_be>
        inline bool operator!=(const Bus<T_be>& l, const Bus<T_be>& r)
        {
            return !(l == r);
        }

        template <unsigned T_be>
        inline ostream& operator<<(ostream& os, const Bus<T_be>& arg)
        {
            char     SP    = '\x020';
            char     NL    = '\n';
            unsigned w     = 3;
            unsigned modn  = (T_be > 4) ? 2 : 1;
            unsigned datn  = (1 << T_be) * 2;
            unsigned bits  = 8 * (1 << T_be);
            unsigned msbh  = bits - 1;
            unsigned lsbh  = bits / 2;
            unsigned msbl  = lsbh - 1;
            string   usrs  = "usr[31:0]";
            string   mods  = "";
            string   daths = "";
            string   datls = "";

            SCDat<T_be> scdat = arg.dat;

            if (T_be == 1U)
            {
                mods  = string("mod");
            }
            else if (T_be != 0U)
            {
                mods  = string("mod[") + to_string(T_be - 1) + ":0]";
            }

            if (T_be == 6)
            {
                daths = string("dat[") + to_string(msbh) + ":" + to_string(lsbh) + "]";
                datls = string("dat[") + to_string(msbl) + ":0]";
            }
            else
            {
                datls = string("dat[") + to_string(msbh) + ":0]";
            }

            if (usrs.size()  > w) { w = usrs.size();  }
            if (mods.size()  > w) { w = mods.size();  }
            if (datls.size() > w) { w = datls.size(); }
            if (daths.size() > w) { w = daths.size(); }

            os << hex;
            os << SP << setw(w) << setfill(SP) << usrs  << SP << "= 0x" << setw(1) << setfill('0') << arg.usr << NL;
            os << SP << setw(w) << setfill(SP) << "err" << SP << "= 0x" << setw(1) << setfill('0') << arg.err << NL;
            os << SP << setw(w) << setfill(SP) << "val" << SP << "= 0x" << setw(1) << setfill('0') << arg.val << NL;
            os << SP << setw(w) << setfill(SP) << "sof" << SP << "= 0x" << setw(1) << setfill('0') << arg.sof << NL;
            os << SP << setw(w) << setfill(SP) << "eof" << SP << "= 0x" << setw(1) << setfill('0') << arg.eof << NL;

            if (T_be > 0)
            {
                os << SP << setw(w) << setfill(SP) << mods  << SP << "= 0x" << setw(modn) << setfill('0') << arg.mod << NL;
            }

            if (T_be == 6)
            {
                unsigned ccnt  = datn / 2;

                os << SP << setw(w) << setfill(SP) << daths  << SP << "= 0x" << setw(ccnt) << setfill('0') ;
                os << scdat.range(msbh, lsbh) << NL;
                os << SP << setw(w) << setfill(SP) << datls  << SP << "= 0x" << setw(ccnt) << setfill('0') ;
                os << scdat.range(msbl, 0) << NL;
            }
            else
            {
                os << SP << setw(w) << setfill(SP) << datls  << SP << "= 0x" << setw(datn) << setfill('0') ;
                os << scdat.range(msbh,0) << NL;
            }

            os << flush;

            return os;
        }

        template <unsigned T_be>
        inline typename std::enable_if<(T_be == 0U), void>::type
        sc_trace(sc_trace_file * tf, const Bus<T_be> & arg, const std::string & nm)
        {
            sc_trace(tf, arg.usr, nm + ".usr");
            sc_trace(tf, arg.err, nm + ".err");
            sc_trace(tf, arg.val, nm + ".val");
            sc_trace(tf, arg.sof, nm + ".sof");
            sc_trace(tf, arg.eof, nm + ".eof");
            sc_trace(tf, arg.dat, nm + ".dat");
        }

        template <unsigned T_be>
        inline typename std::enable_if<(T_be == 1U), void>::type
        sc_trace(sc_trace_file * tf, const Bus<T_be> & arg, const std::string & nm)
        {
            sc_trace(tf, arg.usr, nm + ".usr");
            sc_trace(tf, arg.err, nm + ".err");
            sc_trace(tf, arg.val, nm + ".val");
            sc_trace(tf, arg.sof, nm + ".sof");
            sc_trace(tf, arg.eof, nm + ".eof");
            sc_trace(tf, arg.mod, nm + ".mod");
            sc_trace(tf, arg.dat, nm + ".dat");
        }

        template <unsigned T_be>
        inline typename std::enable_if<(T_be > 1U), void>::type
        sc_trace(sc_trace_file * tf, const Bus<T_be> & arg, const std::string & nm)
        {
            sc_trace(tf, arg.usr, nm + ".usr");
            sc_trace(tf, arg.err, nm + ".err");
            sc_trace(tf, arg.val, nm + ".val");
            sc_trace(tf, arg.sof, nm + ".sof");
            sc_trace(tf, arg.eof, nm + ".eof");
            sc_trace(tf, arg.mod, nm + ".mod");
            sc_trace(tf, arg.dat, nm + ".dat");
        }

        typedef vector<string> str_vec;

        /** \class  Bus_split
         *  \brief  Breaks out individual signals from a Bus
         */

        template <unsigned T_be>
        class Bus_split : public sc_module
        {
            public:
                SC_HAS_PROCESS(Bus_split);
                Bus_split(sc_module_name);
                ~Bus_split(void);

                sc_core::sc_out <uint32_t>  usr_o;
                sc_core::sc_out <bool>      err_o;
                sc_core::sc_out <bool>      val_o;
                sc_core::sc_out <bool>      sof_o;
                sc_core::sc_out <bool>      eof_o;
                sc_core::sc_out <Mod<T_be>> mod_o;
                sc_core::sc_out <Dat<T_be>> dat_o;
                sc_core::sc_in  <Bus<T_be>> bus_i;

                void run(void);
        };

        template <unsigned T_be>
        Bus_split<T_be>::Bus_split(sc_module_name arg_nm)
        {
            SC_METHOD(run);
            sensitive << this->bus_i;
        }

        template <unsigned T_be>
        Bus_split<T_be>::~Bus_split(void) { }

        template <unsigned T_be>
        void Bus_split<T_be>::run(void)
        {
            Bus<T_be> sig_bus = this->bus_i;

            this->usr_o = sig_bus.usr;
            this->err_o = sig_bus.err;
            this->val_o = sig_bus.val;
            this->sof_o = sig_bus.sof;
            this->eof_o = sig_bus.eof;
            this->mod_o = sig_bus.mod;
            this->dat_o = sig_bus.dat;
        }

        /** \class  Bus_src
         *  \brief  Data source for datapath
         *
         *  The driver method Bus_src::drive() places full frames (mod may indicate
         *  non-zero only when the eof indication is asserted) on the output bus.
         *
         *  Other possibly useful driver implementations would be drive_part_frame(),
         *  where mod may be non-zero on any clock cycle.  Another would be
         *  drive_cell() which would drive an ATM cell onto the bus.
         */

        template <unsigned T_be>
        class Bus_src : public sc_module
        {
            private:
                typedef struct struct_frame
                {
                    str_vec  str_bytes;
                    unsigned byte_cnt;
                    unsigned byte_last;
                    unsigned byte_req;
                    uint64_t bit_cnt;
                } frame;

                unique_ptr<SyscMsg::Msg>   msg;
                SyscDrv::DrvClient       * drv;
                string                     drv_handler;
                string                     drv_req;
                frame                    * cur_frm;
                frame                    * nxt_frm;

                void      get_next_frame(void);
                void      frame_swap(void);
                Dat<T_be> get_cur_frame_dat(unsigned);
                Mod<T_be> get_cur_frame_mod(unsigned);
                unsigned  incr_drv_cnt(unsigned);

            public:
                SC_HAS_PROCESS(Bus_src);
                Bus_src(sc_module_name, SyscDrv::DrvClient*, string&, string&);
                ~Bus_src(void);

                sc_core::sc_out <Bus<T_be>> bus_o;
                sc_core::sc_out <bool>      sav_o;
                sc_core::sc_in  <bool>      dav_i;
                sc_core::sc_out <uint32_t>  cnt_o;
                sc_core::sc_out <bool>      req_o;
                sc_core::sc_in  <bool>      ack_i;
                sc_core::sc_in  <bool>      clk_i;
                unsigned                    drv_s;
                unsigned                    drv_c;
                unsigned                    drv_ln;
                unsigned                    drv_lc;

                void     drive(void);
                unsigned get_cur_byte_cnt(void);
                const str_vec& get_cur_byte_vec(void);
        };

        template <unsigned T_be>
        Bus_src<T_be>::Bus_src(sc_module_name arg_nm, SyscDrv::DrvClient * arg_di, string & arg_dh, string & arg_dr)
        {
            this->msg         = unique_ptr<SyscMsg::Msg>(new SyscMsg::Msg(this->name()));
            this->drv         = arg_di;
            this->drv_handler = arg_dh;
            this->drv_req     = arg_dr;
            this->cur_frm     = nullptr;
            this->nxt_frm     = nullptr;

            SC_CTHREAD(drive, this->clk_i.pos());
        }

        template <unsigned T_be>
        Bus_src<T_be>::~Bus_src(void) {
            delete this->cur_frm;
            delete this->nxt_frm;
        }

        template <unsigned T_be>
        unsigned Bus_src<T_be>::get_cur_byte_cnt(void)
        {
            return this->cur_frm->byte_cnt;
        }

        template <unsigned T_be>
        const str_vec & Bus_src<T_be>::get_cur_byte_vec(void)
        {
            return this->cur_frm->str_bytes;
        }

        template <unsigned T_be>
        void Bus_src<T_be>::frame_swap(void)
        {
            delete this->cur_frm;
            this->cur_frm = this->nxt_frm;
            this->nxt_frm = nullptr;
        }

        template <unsigned T_be>
        Mod<T_be> Bus_src<T_be>::get_cur_frame_mod(unsigned arg_cnt)
        {
            Mod<T_be> ret = mod_rst<T_be>();

            if (T_be == 0U)
            {
                return ret;
            }

            unsigned    cnt  = (1 << T_be);
            unsigned    dif  = this->cur_frm->byte_cnt - arg_cnt;

            if (dif < cnt)
            {
                ret = mod_set<T_be>(dif);
            }

            return ret;
        }

        template <unsigned T_be>
        Dat<T_be> Bus_src<T_be>::get_cur_frame_dat(unsigned arg_cnt)
        {
            SCDat<T_be> dat = 0;
            unsigned    idx  = arg_cnt;
            unsigned    cnt  = (1 << T_be);
            unsigned    lim  = idx + cnt;
            unsigned    act  = lim;
            unsigned    msb  = (8 * cnt) - 1;
            unsigned    lsb  = msb - 7;

            if (this->cur_frm->byte_cnt < lim)
            {
                act = this->cur_frm->byte_cnt;
            }

            while (idx < lim)
            {
                if (idx < act)
                {
                    dat.range(msb, lsb) = stoul(this->cur_frm->str_bytes.at(idx), 0, 16);
                }
                else
                {
                    dat.range(msb, lsb) = 0;
                }

                msb = msb - 8;
                lsb = lsb - 8;
                idx++;
            }

            return dat;
        }

        template <unsigned T_be>
        unsigned Bus_src<T_be>::incr_drv_cnt(unsigned arg_cnt)
        {
            unsigned    idx  = arg_cnt;
            unsigned    cnt  = (1 << T_be);
            unsigned    ret  = idx + cnt;

            if (this->cur_frm->byte_cnt < ret)
            {
                ret = this->cur_frm->byte_cnt;
            }

            return ret;
        }

        template <unsigned T_be>
        void Bus_src<T_be>::get_next_frame(void)
        {
            unsigned           bytes            = (1 << T_be);
            string             str_res_data     = "";
            string             str_search       = "";
            string             str_context      = "";
            string             SP               = SyscMsg::Chars::SP;
            SyscJson::JsonFind jfind;
            ostringstream      os;

            if (false)
            {
                this->msg->report_inf("req is" + SP + this->drv_req);
            }

            this->drv->request(str_res_data, this->drv_handler, this->drv_req);

            if (false)
            {
                this->msg->report_inf("res is" + SP + str_res_data);
            }

            os << "{\"frame_len\":true}";
            str_search = os.str();

            try
            {
                jfind.set_search_context(str_res_data);
                jfind.set_search_path(str_search);
            }
            catch (SyscJson::JsonFindErr & err)
            {
                this->msg->cerr_err("[EXPT] SyscFCBus::get_next_frame() JsonFindErr msg:" + SP + err.get_msg());
                throw;
            }

            jfind.find();
            jfind.get_context_string(str_context);

            this->nxt_frm            = new frame;
            this->nxt_frm->byte_cnt  = stoul(str_context, nullptr, 10);
            this->nxt_frm->bit_cnt   = this->nxt_frm->byte_cnt * 8;

            if (bytes > this->nxt_frm->byte_cnt)
            {
                this->nxt_frm->byte_last = 0;
            }
            else
            {
                this->nxt_frm->byte_last = this->nxt_frm->byte_cnt - (1 * bytes);
            }

            if ((2 * bytes) > this->nxt_frm->byte_cnt)
            {
                this->nxt_frm->byte_req = 0;
            }
            else
            {
                this->nxt_frm->byte_req  = this->nxt_frm->byte_cnt - (2 * bytes);
            }

            if (false)
            {
                this->msg->report_inf("next frame_len is" + SP + str_context);
            }

            for (unsigned i = 0 ; i < this->nxt_frm->byte_cnt ; i++)
            {
                os.str("");
                os << "{\"frame\":[" << i << ",true]}";
                str_search = os.str();

                try
                {
                    jfind.set_search_path(str_search);
                }
                catch (SyscJson::JsonFindErr & err)
                {
                    this->msg->cerr_err("[EXPT] SyscFCBus::get_next_frame() JsonFindErr msg:" + SP + err.get_msg());
                    throw;
                }

                jfind.find();
                jfind.get_context_string(str_context);

                this->nxt_frm->str_bytes.push_back(str_context);
            }
        }

        template <unsigned T_be>
        void Bus_src<T_be>::drive(void)
        {
            enum enum_drv_fsm
            {
                state_init,
                state_req,
                state_ack,
                state_pen,
                state_LAST
            };

            string       SP           = SyscMsg::Chars::SP;
            unsigned     drv_cnt      = 0;
            Bus<T_be>    sig_bus      = bus_rst<T_be>();
            uint64_t     sig_cnt      = 0;
            bool         sig_req      = false;
            bool         sig_ack      = false;
            enum_drv_fsm drv_state    = state_init;

            this->bus_o  = sig_bus;
            this->req_o  = sig_req;
            this->cnt_o  = sig_cnt;
            this->sav_o  = true;
            this->drv_s  = drv_state;
            this->drv_c  = drv_cnt;
            this->drv_lc = 0;
            this->drv_ln = 0;

            this->msg->report_inf("started SyscFCBus::Bus_src()");

            while (true)
            {
                wait();

                if (this->dav_i == false)
                {
                    continue;
                }

                sig_ack     = this->ack_i;
                sig_bus.sof = false;
                sig_bus.eof = false;

                switch (drv_state)
                {
                    case state_init:
                    {
                        this->get_next_frame();

                        sig_cnt     = this->nxt_frm->bit_cnt;
                        sig_req     = true;
                        sig_bus.dat = 0;
                        sig_bus.mod = mod_rst<T_be>();
                        sig_bus.val = false;
                        drv_state   = state_req;

                        break;
                    }
                    case state_req:
                    {
                        if (!sig_ack)
                        {
                            sig_req     = true;
                            sig_bus.val = false;
                            break;
                        }

                        this->frame_swap();

                        drv_cnt     = 0;
                        sig_bus.mod = get_cur_frame_mod(drv_cnt);
                        sig_bus.dat = get_cur_frame_dat(drv_cnt);
                        sig_bus.sof = true;

                        if (drv_cnt >= this->cur_frm->byte_req)
                        {
                            this->get_next_frame();

                            sig_cnt     = this->nxt_frm->bit_cnt;
                            sig_req     = true;
                        }
                        else
                        {
                            sig_req     = false;
                        }

                        if (drv_cnt >= this->cur_frm->byte_last)
                        {
                            sig_bus.eof = true;
                            sig_bus.val = true;
                            drv_state   = state_req;
                        }
                        else if (drv_cnt >= this->cur_frm->byte_req)
                        {
                            sig_bus.val = true;
                            drv_state   = state_pen;
                        }
                        else
                        {
                            sig_bus.val = true;
                            drv_state   = state_ack;
                        }

                        if (false)
                        {
                            this->msg->report_inf
                            (
                                "SyscFCBus::drive() state_req drv_cnt cur_byte_req cur_byte_last:"
                                + SP + to_string(drv_cnt)
                                + SP + to_string(this->cur_frm->byte_req)
                                + SP + to_string(this->cur_frm->byte_last)
                            );
                        }

                        break;
                    }
                    case state_ack:
                    {
                        drv_cnt     = incr_drv_cnt(drv_cnt);
                        sig_bus.mod = get_cur_frame_mod(drv_cnt);
                        sig_bus.dat = get_cur_frame_dat(drv_cnt);

                        if (drv_cnt >= this->cur_frm->byte_req)
                        {
                            this->get_next_frame();

                            sig_cnt     = this->nxt_frm->bit_cnt;
                            sig_req     = true;
                        }
                        else
                        {
                            sig_req     = false;
                        }

                        if (drv_cnt >= this->cur_frm->byte_last)
                        {
                            sig_bus.eof = true;
                            sig_bus.val = true;
                            drv_state   = state_req;
                        }
                        else if (drv_cnt >= this->cur_frm->byte_req)
                        {
                            sig_bus.val = true;
                            drv_state   = state_pen;
                        }
                        else
                        {
                            sig_bus.val = true;
                            drv_state   = state_ack;
                        }

                        if (false)
                        {
                            this->msg->report_inf
                            (
                                "SyscFCBus::drive() state_ack drv_cnt cur_byte_req cur_byte_last:"
                                + SP + to_string(drv_cnt)
                                + SP + to_string(this->cur_frm->byte_req)
                                + SP + to_string(this->cur_frm->byte_last)
                            );
                        }

                        break;
                    }
                    case state_pen:
                    {
                        drv_cnt     = incr_drv_cnt(drv_cnt);
                        sig_bus.mod = get_cur_frame_mod(drv_cnt);
                        sig_bus.dat = get_cur_frame_dat(drv_cnt);

                        if (drv_cnt >= this->cur_frm->byte_last)
                        {
                            sig_bus.eof = true;
                            sig_bus.val = true;
                            drv_state   = state_req;
                        }
                        else
                        {
                            sig_bus.val = true;
                            drv_state   = state_pen;
                        }

                        if (false)
                        {
                            this->msg->report_inf
                            (
                                "SyscFCBus::drive() state_pen drv_cnt cur_byte_req cur_byte_last:"
                                + SP + to_string(drv_cnt)
                                + SP + to_string(this->cur_frm->byte_req)
                                + SP + to_string(this->cur_frm->byte_last)
                            );
                        }

                        break;
                    }
                    default:
                    {
                        this->msg->report_inf("FSM in default case");
                        throw "Bus instance" + SP + this->msg->get_str_c_msgid() + SP + "drive() FSM in default case";
                        break;
                    }
                }

                this->bus_o = sig_bus;
                this->cnt_o = sig_cnt;
                this->req_o = sig_req;
                this->drv_s = drv_state;
                this->drv_c = drv_cnt;

                if (cur_frm != nullptr) { this->drv_lc = this->cur_frm->byte_cnt; }
                if (nxt_frm != nullptr) { this->drv_ln = this->nxt_frm->byte_cnt; }
            }
        }
    }
#endif
