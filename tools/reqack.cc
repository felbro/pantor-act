//
// Simple request ack'er
//

#include "Dec.h"
#include "Enc.h"

using namespace ACT;

struct Ack : public MsgObserver
{
   Ack () : serverOrderId (4700128) { }
   ~Ack () { }
   
   void setBinaryOutput ()
   {
      enc.setBinary ();
   }

   void onSystemStatus (const Public::SystemStatus & m) override { }
   void onInstrumentInfo (const Public::InstrumentInfo & m) override { }
   void onInstrumentStatus (const Public::InstrumentStatus & m) override { }
   void onIndexInfo (const Public::IndexInfo & m) override { }
   void onIndexMemberInfo (const Public::IndexMemberInfo & m) override { }
   void onIndexUpdate (const Public::IndexUpdate & m) override { }
   void onTopOfBook (const Public::TopOfBook & m) override { }
   void onTradeStats (const Public::TradeStats & m) override { }
   void onOrderInserted (const Public::OrderInserted & m) override { }
   void onOrderDeleted (const Public::OrderDeleted & m) override { }
   void onOrderReduced (const Public::OrderReduced & m) override { }
   void onOrderReplaced (const Public::OrderReplaced & m) override { }
   void onOrderExecuted (const Public::OrderExecuted & m) override { }
   void onInsertOrderReply (const Private::InsertOrderReply & m) override { }
   void onDeleteOrderReply (const Private::DeleteOrderReply & m) override { }
   void onReduceOrderReply (const Private::ReduceOrderReply & m) override { }
   void onReplaceOrderReply (const Private::ReplaceOrderReply & m) override { }
   void onRejectOrderReply (const Private::RejectOrderReply & m) override { }

   void
   onInsertOrder (const Private::InsertOrder & m) override
   {
      Private::InsertOrderReply r;

      r.instrumentId = m.instrumentId;
      r.clientId = m.clientId;
      r.clientOrderId = m.clientOrderId;
      r.serverOrderId = ++ serverOrderId;
      r.price = m.price;
      r.quantity = m.quantity;
      r.side = m.side;
      r.tif = m.tif;
      
      enc.send (r);
   }
   
   void
   onDeleteOrder (const Private::DeleteOrder & m) override
   {
      Private::DeleteOrderReply r;

      r.instrumentId = m.instrumentId;
      r.clientId = m.clientId;
      r.clientOrderId = m.clientOrderId;

      enc.send (r);
   }
   
   void
   onReduceOrder (const Private::ReduceOrder & m) override
   {
      Private::ReduceOrderReply r;

      r.instrumentId = m.instrumentId;
      r.clientId = m.clientId;
      r.clientOrderId = m.clientOrderId;
      r.quantity = 255; // don't know the initial qty in this case

      enc.send (r);
   }
   
   void
   onReplaceOrder (const Private::ReplaceOrder & m) override
   {
      Private::ReplaceOrderReply r;

      r.instrumentId = m.instrumentId;
      r.clientId = m.clientId;
      r.oldClientOrderId = m.clientOrderId;
      r.newClientOrderId = m.newClientOrderId;

      r.serverOrderId = ++ serverOrderId;
      r.price = m.price;
      r.quantity = m.quantity;

      enc.send (r);
   }

   u64 serverOrderId;
   Encoder enc;
};

static void
usage (const char* cmd)
{
   std::cerr << "usage: " << cmd << " b|t b|t"  << std::endl;
   std::cerr << "  b/t (binary/text input)" << std::endl;
   std::cerr << "  b/t (binary/text output)" << std::endl;
   exit (0);
}

int
main (int ac, char** av)
{
   if (ac != 3)
      usage (av [0]);
   
   Ack a;

   // input:
   bool binInput = false;

   switch (* av [1])
   {
    case 'b': binInput = true; break;
    case 't': binInput = false; break;
      default: usage (av [0]);
   }

   // output:
   bool binOutput = false;
   switch (* av [2])
   {
    case 'b': binOutput = true; break;
    case 't': binOutput = false; break;
      default: usage (av [0]);
   }

   if (binOutput)
      a.setBinaryOutput ();

   if (binInput)
   {
      std::cerr << "Decoder: Binary..." << std::endl;
      ACT::BinDecoder d (a);
      d.run ();
   }
   else
   {
      std::cerr << "Decoder: Text..." << std::endl;
      ACT::TextDecoder d (a);
      d.run ();
   }

   return 0;
}
