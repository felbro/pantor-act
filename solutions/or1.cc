/*
   Solution to OR1: Stateless Order Validation.

   Inspiration taken from md1.cc (mainly the main-function)

   @author Felix Broberg
   @version 2016-10-24
 */
#include "Dec.h"
#include "Enc.h"
#include <map>
#include <stdlib.h>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <functional>


using namespace ACT;



struct StatsObserver : public MsgObserver
{

        std::map<u32, bool> instruments;

        std::map<u32, Public::TopOfBook> TOB;

        std::unordered_set<u32> orders;


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

        void onInsertOrder (const Private::InsertOrder & m) override {
                //1
                const auto iter = instruments.find (m.instrumentId);
                if (iter == instruments.end ())
                {
                        reject(m, RejectReason::InvalidInstrument);
                        return;
                }

                //2
                if (m.price < 0) {
                        reject(m, RejectReason::InvalidPrice);
                        return;
                }
                //3
                if (m.quantity < 0) {
                        reject(m, RejectReason::InvalidQuantity);
                        return;
                }
                //4

                const auto it = orders.find (m.clientOrderId);
                if (it != orders.end ())
                {
                        reject(m, RejectReason::InvalidOrderId);
                        return;
                }


                //5
                if(m.price * m.quantity >= 10000000000) {
                        reject(m, RejectReason::InvalidNotionalValue);
                        return;
                }

                //6
                if (m.side == Side::Buy) {
                        double spread = fabs (((double)TOB [m.instrumentId].askPrice - m.price) / TOB [m.instrumentId].askPrice);

                        if(spread >= 0.1) {
                                reject(m, RejectReason::NotionalLimitExceeded);
                                return;
                        }
                }
                if (m.side == Side::Sell) {
                        double spread = fabs (((double)TOB [m.instrumentId].bidPrice - m.price) / TOB [m.instrumentId].bidPrice);
                        if(spread >= 0.1) {
                                reject(m, RejectReason::NotionalLimitExceeded);
                                return;
                        }
                }

                orders.insert(m.clientOrderId);
                enc.send(m);
        }

        void reject(const Private::InsertOrder & m, RejectReason r) {
                Private::RejectOrderReply ror;
                ror.instrumentId = m.instrumentId;
                ror.clientId = m.clientId;
                ror.clientOrderId = m.clientOrderId;
                ror.reason = r;
                enc.send(ror);
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
        void onOrderInserted (const Public::OrderInserted & m) override {

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
