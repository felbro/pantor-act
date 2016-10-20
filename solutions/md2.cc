/*
   Solution to MD2: Top of Book.

   Inspiration taken from md1.cc (mainly the main-function)

   @author Felix Broberg
   @version 2016-10-17
 */
#include "Dec.h"
#include "Enc.h"
#include <map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <functional>
#include <stdlib.h>

using namespace ACT;



struct StatsObserver : public MsgObserver
{
        bool trace;
        // instrumentId mapped to name
        //std::map<u32, std::string> instruments;
        std::unordered_set<u32> instruments;

        //InstrumentId mapped to corresponding TOB
        std::map<u32, Public::TopOfBook> IS;

        // serverOrderId mapped order
        std::map<u64, Public::OrderInserted> OS;
        // instrumentId mapped to all orders.
        typedef std::map<u32, std::vector<u64> > InstrumentOrders;
        InstrumentOrders IObuy;
        InstrumentOrders IOsell;

        Encoder enc;

        static bool greater(u64 a,u64 b){
                return a>b;
        }
        static bool lesser(u64 a,u64 b){
                return a<b;
        }


        StatsObserver () {
        }
        ~StatsObserver () {
        }

        void setBinaryOutput ()
        {
                enc.setBinary ();
        }

        void
        onSystemStatus (const Public::SystemStatus & m) override
        {
                if (!m.open) {
                        std::cerr << "The system needs to be opened: " << m.toString()<<std::endl;
                }

                else
                {
                        instruments.clear();

                        IS.clear();
                        OS.clear();
                        IObuy.clear();
                        IOsell.clear();
                }
        }

        void
        onInstrumentInfo (const Public::InstrumentInfo & m) override
        {

                instruments.insert(m.instrumentId);

                Public::TopOfBook s;
                IS [m.instrumentId] = s;
        }
        void orderError(const Public::OrderInserted & m){
                std::cerr << "ERROR: Unknown instrument: " << m.toString () << std::endl;
        }


        void onOrderInserted (const Public::OrderInserted & m) override {
                const auto iter = instruments.find (m.instrumentId);
                if (iter == instruments.end ())
                {
                        orderError(m);
                        abort ();
                }


                OS [m.serverOrderId] = m;

                if(m.side == Side::Buy)
                        onInserted(m, IObuy [m.instrumentId], IS [m.instrumentId].bidPrice, IS [m.instrumentId].bidQuantity,lesser);

                else
                        onInserted(m, IOsell [m.instrumentId], IS [m.instrumentId].askPrice, IS [m.instrumentId].askQuantity,greater);
        }
        void onInserted(Public::OrderInserted m, std::vector<u64> & vec, u64 & p, u64 & q,std::function<bool (u64,u64)> func){
                if (func(p,m.price) || p == 0 || q == 0) {
                        p = m.price;
                        q = m.quantity;
                        enc.send (IS [m.instrumentId]);
                }
                else if (p == m.price ) {
                        q += m.quantity;
                        enc.send (IS [m.instrumentId]);
                }

                vec.push_back(m.serverOrderId);

        }


        void onOrderDeleted (const Public::OrderDeleted & m) override {


                const auto it = OS.find (m.serverOrderId);
                if (it == OS.end ())
                {
                        std::cerr << "Error: Unknown order: " << m.toString() << std::endl;
                        abort ();
                }


                if (OS[m.serverOrderId].instrumentId != m.instrumentId) {
                        std::cerr << "Error: Instrument Info for given order does not correspond with message Instrument Info: " <<
                        m.toString() << std::endl;
                        std::cerr << OS[m.serverOrderId].instrumentId << " != " << m.instrumentId << std::endl;
                        abort();
                }

                auto sortbyPriceDesc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price > OS[rhs].price;
                };

                auto sortbyPriceAsc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price < OS[rhs].price;
                };


                if (OS [m.serverOrderId].side == Side::Buy) {

                        sort(IObuy [m.instrumentId].begin(),IObuy [m.instrumentId].end(),sortbyPriceDesc);
                        toErase(m, IObuy [m.instrumentId], IS [m.instrumentId].bidPrice, IS [m.instrumentId].bidQuantity);
                }
                else{
                        sort(IOsell [m.instrumentId].begin(),IOsell [m.instrumentId].end(),sortbyPriceAsc);

                        toErase(m, IOsell [m.instrumentId], IS [m.instrumentId].askPrice, IS [m.instrumentId].askQuantity);
                }

                OS.erase (m.serverOrderId);

        }

        void toErase(Public::OrderDeleted m, std::vector<u64> & vec, u64 & p, u64 & q){

                if (OS [m.serverOrderId].price == p) {

                        q -= OS [m.serverOrderId].quantity;
                        removeItem(m.serverOrderId,vec);
                        if(q == 0) decreaseAndReplace(m.serverOrderId,vec,p,q);


                        enc.send(IS [m.instrumentId]);
                }
                else {
                        removeItem(m.serverOrderId,vec);
                }

        }
        void removeItem(u64 id, std::vector<u64> & vec) {
                for (unsigned int i = 0; i < vec.size(); i++) {
                        if (OS[vec[i]].serverOrderId == id) {
                                vec.erase(vec.begin()+i);
                                break;
                        }
                }
        }

        void decreaseAndReplace(u64 id, std::vector<u64> & vec, u64 & p, u64 & q){
                if(vec.size() > 1) {

                        unsigned int i = 0;
                        u64 tempq = 0;
                        u64 tempp = 0;
                        while(i < vec.size() ) {

                                if (OS[vec[i]].quantity != 0 && i < vec.size() && OS[vec[i]].serverOrderId != id) {
                                        tempp = OS[vec[i]].price;
                                        while(OS[vec[i]].price == tempp && i < vec.size()) {
                                                tempq += OS[vec[i]].quantity;
                                                i++;
                                        }

                                        if(tempq > 0) {
                                                p = tempp;
                                                q = tempq;
                                                return;
                                        }


                                }
                                tempq = 0;
                                i++;
                        }

                }

                p = 0;
                q = 0;
        }


        void onOrderExecuted (const Public::OrderExecuted & m) override {
                // error messages
                const auto it = OS.find (m.serverOrderId);
                if (it == OS.end ())
                {
                        std::cerr << "Error: Unknown order: " << m.toString() << std::endl;
                        abort ();
                }
                if (OS[m.serverOrderId].instrumentId != m.instrumentId) {
                        std::cerr << "Error: Instrument Info for given order does not correspond with message Instrument Info: " <<
                        m.toString() << std::endl;
                        std::cerr << OS[m.serverOrderId].instrumentId << " != " << m.instrumentId << std::endl;
                        abort();
                }
                if (OS[m.serverOrderId].price != m.price) {
                        std::cerr << "Error: The excecuted price does not equal the order price: " << m.toString() << std::endl;
                        std::cerr << OS[m.serverOrderId].price << " != "<< m.price << std::endl;
                        abort();
                }
                if (OS[m.serverOrderId].quantity < m.quantity) {
                        std::cerr << "Error: The excecuted quantity exceeds the order quantity: " << m.toString() << std::endl;
                        std::cerr << OS[m.serverOrderId].quantity << " < "<< m.quantity << std::endl;
                        abort();
                }

                auto sortbyPriceDesc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price > OS[rhs].price;
                };

                auto sortbyPriceAsc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price < OS[rhs].price;
                };


                OS[m.serverOrderId].quantity -= m.quantity;


                if (OS [m.serverOrderId].side == Side::Buy) {
                        sort(IObuy [m.instrumentId].begin(),IObuy [m.instrumentId].end(),sortbyPriceDesc);
                        toExecute(m, IObuy [m.instrumentId], IS [m.instrumentId].bidPrice, IS [m.instrumentId].bidQuantity);
                }
                else{
                        sort(IOsell [m.instrumentId].begin(),IOsell [m.instrumentId].end(),sortbyPriceAsc);
                        toExecute(m, IOsell [m.instrumentId], IS [m.instrumentId].askPrice, IS [m.instrumentId].askQuantity);
                }
                if(OS[m.serverOrderId].quantity == 0) OS.erase(m.serverOrderId);

        }

        void toExecute(Public::OrderExecuted m, std::vector<u64> & vec, u64 & p, u64 & q){
                if (OS [m.serverOrderId].price == p) {
                        q -= m.quantity;

                        if(q == 0) decreaseAndReplace(m.serverOrderId,vec,p,q);
                        if(OS[m.serverOrderId].quantity == 0) {
                                removeItem(m.serverOrderId,vec);

                        }
                        enc.send(IS [m.instrumentId]);
                }
        }


        void onOrderReplaced (const Public::OrderReplaced & m ) override {

                // error messages
                const auto it = OS.find (m.serverOrderId);
                if (it == OS.end ())
                {
                        std::cerr << "Error: Unknown order: " << m.toString() << std::endl;
                        abort ();
                }
                if (OS[m.serverOrderId].instrumentId != m.instrumentId) {
                        std::cerr << "Error: Instrument Info for given order does not correspond with message Instrument Info: " <<
                        m.toString() << std::endl;
                        std::cerr << OS[m.serverOrderId].instrumentId << " != " << m.instrumentId << std::endl;
                        abort();
                }

                auto sortbyPriceDesc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price > OS[rhs].price;
                };

                auto sortbyPriceAsc = [&](const u64 lhs,const u64 rhs)->bool {

                        return OS[lhs].price < OS[rhs].price;
                };

                OS[m.newServerOrderId] = OS[m.serverOrderId];
                OS[m.newServerOrderId].price = m.price;
                OS[m.newServerOrderId].quantity = m.quantity;


                if (OS [m.serverOrderId].side == Side::Buy) {

                        sort(IObuy [m.instrumentId].begin(),IObuy [m.instrumentId].end(),sortbyPriceDesc);
                        toReplace(m, IObuy [m.instrumentId], IS [m.instrumentId].bidPrice, IS [m.instrumentId].bidQuantity,greater);
                }
                else{
                        sort(IOsell [m.instrumentId].begin(),IOsell [m.instrumentId].end(),sortbyPriceAsc);
                        toReplace(m, IOsell [m.instrumentId], IS [m.instrumentId].askPrice, IS [m.instrumentId].askQuantity,lesser);
                }
                OS.erase(m.serverOrderId);


        }

        void toReplace(Public::OrderReplaced m, std::vector<u64> & vec, u64 & p, u64 & q,std::function<bool (u64,u64)> better){

                vec.push_back(m.newServerOrderId);
                if (better(m.price,p)) {
                        p = m.price;
                        q = m.quantity;
                        enc.send(IS [m.instrumentId]);
                }

                else if (m.price == p) {

                        if ( p == OS[m.serverOrderId].price) q -= OS[m.serverOrderId].quantity;
                        q += m.quantity;

                        if(q == 0) decreaseAndReplace(m.serverOrderId,vec,p,q);
                        if(OS[m.serverOrderId].price != m.price || OS[m.serverOrderId].quantity != m.quantity)
                                enc.send(IS [m.instrumentId]);
                }

                else {

                        if (p == OS[m.serverOrderId].price) {

                                q -= OS[m.serverOrderId].quantity;

                                if(q == 0) decreaseAndReplace(m.serverOrderId,vec,p,q);


                                enc.send(IS [m.instrumentId]);
                        }
                }

                removeItem(m.serverOrderId,vec);


        }



        // ignored:

        void onOrderReduced (const Public::OrderReduced &) override {
        }
        void onInstrumentStatus (const Public::InstrumentStatus &) override {
        }
        void onIndexInfo (const Public::IndexInfo &) override {
        }
        void onIndexMemberInfo (const Public::IndexMemberInfo &) override {
        }
        void onIndexUpdate (const Public::IndexUpdate &) override {
        }
        void onTopOfBook (const Public::TopOfBook &) override {
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
