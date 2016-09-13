//
// Solution to task md1: Trade Stats
//

#include "Dec.h"
#include "Enc.h"
#include <map>

using namespace ACT;

struct StatsObserver : public MsgObserver
{

   StatsObserver () { }
   ~StatsObserver () { }

   void setBinaryOutput ()
   {
      enc.setBinary ();
   }

   void
   onInstrumentInfo (const Public::InstrumentInfo & m) override
   {
      instruments [m.instrumentId] = m.name;
      DTV [m.instrumentId] = 0;
      VWAP [m.instrumentId] = 0;
   }

   void
   onOrderExecuted (const Public::OrderExecuted & m) override
   {
      const auto iter = instruments.find (m.instrumentId);
      if (iter == instruments.end ())
      {
         std::cerr << "ERROR: Unknown instrument: "
                   << m.toString () << std::endl;
         abort ();
      }
      VWAP[m.instrumentId] = ((DTV[m.instrumentId] * VWAP[m.instrumentId]) + (m.price * m.quantity)) / (DTV[m.instrumentId] + m.quantity);
      DTV[m.instrumentId] += m.quantity;

      Public::TradeStats s;
      s.instrumentId = m.instrumentId;
      s.lastPrice = m.price;
      s.lastQuantity = m.quantity;
      s.vwapPrice = VWAP[m.instrumentId];

      enc.send (s);
   }

   void
   onSystemStatus (const Public::SystemStatus & m) override
   {
      if (!m.open) {
        for (auto it = DTV.cbegin(); it != DTV.cend(); ++it)
        {
          std::cerr << "DTV: " << it->first << "-" << it->second << std::endl;
        }
        for (auto it = VWAP.cbegin(); it != VWAP.cend(); ++it)
        {
          std::cerr << "VWAP: " << it->first << "-" << it->second << std::endl;
        }
      }
      else
      {
        DTV.clear();
        instruments.clear();
        VWAP.clear();
      }
   }
   
   // ignored:
   void onOrderInserted (const Public::OrderInserted &) override { }
   void onOrderDeleted (const Public::OrderDeleted &) override { }
   void onOrderReduced (const Public::OrderReduced &) override { }
   void onOrderReplaced (const Public::OrderReplaced &) override { }
   void onInstrumentStatus (const Public::InstrumentStatus &) override { }
   void onIndexInfo (const Public::IndexInfo &) override { }
   void onIndexMemberInfo (const Public::IndexMemberInfo &) override { }
   void onIndexUpdate (const Public::IndexUpdate &) override { }
   void onTopOfBook (const Public::TopOfBook &) override { }
   void onTradeStats (const Public::TradeStats &) override { }
   void onInsertOrderReply (const Private::InsertOrderReply &) override { }
   void onDeleteOrderReply (const Private::DeleteOrderReply &) override { }
   void onReduceOrderReply (const Private::ReduceOrderReply &) override { }
   void onReplaceOrderReply (const Private::ReplaceOrderReply &) override { }
   void onRejectOrderReply (const Private::RejectOrderReply &) override { }
   void onInsertOrder (const Private::InsertOrder &) override { }
   void onDeleteOrder (const Private::DeleteOrder &) override { }
   void onReduceOrder (const Private::ReduceOrder &) override { }
   void onReplaceOrder (const Private::ReplaceOrder &) override { }

   bool trace;

   typedef std::map<u32, std::string> Instruments;
   Instruments instruments;
   typedef std::map<u32, u64> DailyTradedVolume;
   DailyTradedVolume DTV;
   typedef std::map<u32, u64> VolumeWeightedAvaragePrice;
   VolumeWeightedAvaragePrice VWAP;

   Encoder enc;
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
   
   if (* av [1] == 't')
   {
      std::cerr << "Decoder: Text..." << std::endl;
      TextDecoder d (s);
      d.run ();
   }
   else if (* av [1] == 'b')
   {
      std::cerr << "Decoder: Bin..." << std::endl;
      BinDecoder d (s);
      d.run ();
   }
   else
      usage (av [0]);

   return 0;
}
