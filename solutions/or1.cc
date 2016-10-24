/*
   Solution to OR1: Stateless Order Validation.

   Inspiration taken from md1.cc (mainly the main-function)

   @author Felix Broberg
   @version 2016-10-24
 */
#include "Dec.h"
#include "Enc.h"
#include <map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <functional>


using namespace ACT;



struct StatsObserver : public MsgObserver
{

        std::map<u32, bool> instruments;

        std::map<u32, Public::TopOfBook> TOB;

        Encoder enc;



        StatsObserver () {
        }
        ~StatsObserver () {
        }

        void setBinaryOutput ()
        {
                enc.setBinary ();
        }



        void onInstrumentInfo (const Public::InstrumentInfo & m) override {
                instruments[m.instrumentId] = 0;
        }



        void onInstrumentStatus (const Public::InstrumentStatus & m) override {
                const auto iter = instruments.find (m.instrumentId);
                if (iter == instruments.end ())
                {
                        std::cerr << "ERROR: Unknown instrument: " << m.toString () << std::endl;
                        abort ();
                }

                instruments[m.instrumentId] = m.active;
        }



        void onTopOfBook (const Public::TopOfBook & m) override {
                const auto iter = instruments.find (m.instrumentId);
                if (iter == instruments.end ())
                {
                        std::cerr << "ERROR: Unknown instrument: " << m.toString () << std::endl;
                        abort ();
                }

                TOB [m.instrumentId] = m;
        }



        void onOrderInserted (const Public::OrderInserted & m) override {

        }






        // ignored:
        void onOrderDeleted (const Public::OrderDeleted &) override {
        }
        void onOrderExecuted (const Public::OrderExecuted &) override {
        }
        void onOrderReplaced (const Public::OrderReplaced & ) override {
        }
        void onSystemStatus (const Public::SystemStatus &) override {
        }
        void onOrderReduced (const Public::OrderReduced &) override {
        }

        void onIndexInfo (const Public::IndexInfo &) override {
        }
        void onIndexMemberInfo (const Public::IndexMemberInfo &) override {
        }
        void onIndexUpdate (const Public::IndexUpdate &) override {
        }

        void onTradeStats (const Public::TradeStats &) override {
        }
        void onInsertOrderReply (const Private::InsertOrderReply &) override {
        }
        void onDeleteOrderReply (const Private::DeleteOrderReply &) override {
        }
        void onReduceOrderReply (const Private::ReduceOrderReply &) override {
        }
        void onReplaceOrderReply (const Private::ReplaceOrderReply &) override {
        }
        void onRejectOrderReply (const Private::RejectOrderReply &) override {
        }
        void onInsertOrder (const Private::InsertOrder &) override {
        }
        void onDeleteOrder (const Private::DeleteOrder &) override {
        }
        void onReduceOrder (const Private::ReduceOrder &) override {
        }
        void onReplaceOrder (const Private::ReplaceOrder &) override {
        }


};

static void
usage (const char* cmd)
{
        std::cerr << "usage: " << cmd << " (b | t) (b | t)" << std::endl;
        std::cerr << "  b (binary input)" << std::endl;
        std::cerr << "  t (text input)" << std::endl;
        exit (0);
}

int
main (int ac, char** av)
{
        StatsObserver s;


        if (ac != 3)
                usage (av [0]);

        if (*av [2] == 'b') {
                s.setBinaryOutput();
        }

        if (*av [1] == 't')
        {
                std::cerr << "Decoder: Text..." << std::endl;
                TextDecoder d (s);
                d.run ();
        }
        else if (*av [1] == 'b')
        {
                std::cerr << "Decoder: Bin..." << std::endl;
                BinDecoder d (s);
                d.run ();
        }

        else
                usage (av [0]);

        return 0;
}
