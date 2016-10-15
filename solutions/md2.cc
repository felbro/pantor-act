//
// Solution to task md1: Trade Stats
//

#include "Dec.h"
#include "Enc.h"
#include <map>
#include <algorithm>
#include <vector>

using namespace ACT;


struct InstrumentStats
{
        u64 bbp = 0;
        u64 tbq = 0;
        u64 bap = 0;
        u64 taq = 0;


};

/*struct OrdersByInstrument {
        std::vector<Public::OrderDeleted> a;
   };*/



struct StatsObserver : public MsgObserver
{

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
                        std::cerr << "Lawl error" << std::endl;
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
                instruments [m.instrumentId] = m.name;
                InstrumentStats i;
                IS [m.instrumentId] = i;
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


                if(m.side == Side::Buy) {




                        s.instrumentId = m.instrumentId;
                        if ((IS [m.instrumentId].bbp < m.price) || IObuy[m.instrumentId].empty()) {
                                IS [m.instrumentId].bbp = m.price;
                                IS [m.instrumentId].tbq = m.quantity;
                                s.bidPrice = m.price;
                                s.bidQuantity = m.quantity;
                        }
                        else if (IS [m.instrumentId].bbp == m.price ) {
                                IS [m.instrumentId].tbq += m.quantity;
                                s.bidQuantity = IS [m.instrumentId].tbq;
                        }
                        IObuy [m.instrumentId].push_back(m);
                        enc.send (s);

                }

                else {

                        s.instrumentId = m.instrumentId;

                        if((IS [m.instrumentId].bap > m.price) || IOsell[m.instrumentId].empty()) {
                                IS [m.instrumentId].bap = m.price;
                                IS [m.instrumentId].taq = m.quantity;
                                s.askPrice = m.price;
                                s.askQuantity = m.quantity;
                        }
                        else if (IS [m.instrumentId].bap == m.price) {
                                IS [m.instrumentId].taq += m.quantity;
                                s.askQuantity = IS [m.instrumentId].taq;
                        }
                        IOsell [m.instrumentId].push_back(m);
                        enc.send (s);

                }

        }

        static bool sortbyPriceDesc(const Public::OrderInserted &lhs, const Public::OrderInserted &rhs) {
                return lhs.price > rhs.price;
        }

        static bool sortbyPriceAsc(const Public::OrderInserted &lhs, const Public::OrderInserted &rhs) {
                return lhs.price < rhs.price;
        }

        void onOrderDeleted (const Public::OrderDeleted & m) override {

                const auto it = OS.find (m.serverOrderId);
                if (it == OS.end ())
                {
                        std::cerr << "Error: Unknown order: " << m.toString() << std::endl;
                        abort ();
                }
                if (OS [m.serverOrderId].side == Side::Buy) {
                        sort(IObuy [m.instrumentId].begin(),IObuy [m.instrumentId].end(),sortbyPriceDesc);
                        toErase(m, IObuy [m.instrumentId], IS [m.instrumentId].bbp, IS [m.instrumentId].tbq, OS [m.serverOrderId].side);
                }
                else{
                        sort(IOsell [m.instrumentId].begin(),IOsell [m.instrumentId].end(),sortbyPriceAsc);
                        toErase(m, IOsell [m.instrumentId], IS [m.instrumentId].bap, IS [m.instrumentId].taq, OS [m.serverOrderId].side);
                }

                OS.erase (m.serverOrderId);

        }

        void toErase(Public::OrderDeleted m, std::vector<Public::OrderInserted> & vec, u64 & p, u64 & q, Side side){
                if (OS [m.serverOrderId].price == p) {

                        q -= OS [m.serverOrderId].quantity;

                        if(q == 0) {

                                if(vec.size() > 1) {

                                        p = vec[1].price;
                                        u64 tmp = 0;
                                        unsigned int i = 1;

                                        while(vec[i].price == p && i < vec.size()) {
                                                tmp += vec[i].quantity;
                                                i++;
                                        }
                                        q = tmp;

                                }
                                else p = 0;
                                if(side == Side::Buy) s.bidPrice = p;
                                else s.askPrice = p;
                        }
                        vec.erase(vec.begin());

                        if(side == Side::Buy) s.bidQuantity = q;
                        else s.askQuantity = q;

                        enc.send(s);
                }
                else {
                        for (unsigned i = 0; i < vec.size(); i++) {
                                if (vec[i].serverOrderId == m.serverOrderId) {
                                        vec.erase(vec.begin()+i);
                                        break;
                                }
                        }
                }
        }


        void onOrderReplaced (const Public::OrderReplaced & m ) override {
        }



        void
        onOrderExecuted (const Public::OrderExecuted & m) override
        {

        }


        bool trace;

        typedef std::map<u32, std::string> Instruments;
        Instruments instruments;
        typedef std::map<u32, InstrumentStats> InStats;
        InStats IS;
        typedef std::map<u64, Public::OrderInserted> Orders;
        Orders OS;
        typedef std::map<u32, std::vector<Public::OrderInserted> > InstrumentOrders;
        InstrumentOrders IObuy;
        InstrumentOrders IOsell;
        Public::TopOfBook s;


        Encoder enc;



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
        std::cerr << "usage: " << cmd << " b | t" << std::endl;
        std::cerr << "  b (binary input)" << std::endl;
        std::cerr << "  t (text input)" << std::endl;
        exit (0);
}

int
main (int ac, char** av)
{
        StatsObserver s;

        if (ac != 2)
                usage (av [0]);

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
