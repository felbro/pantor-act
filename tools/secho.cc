//
// read server messages (bin or text), write a copy (bin or text)
//

#include "Dec.h"
#include "Enc.h"

using namespace ACT;

struct MyObserver : public MsgObserver
{
   MyObserver () : trace (false) { }
   ~MyObserver () { }

   void enableTrace ()
   {
      trace = true;
      enc.enableTrace ();
   }

   void setBinaryOutput ()
   {
      enc.setBinary ();
   }

   void
   onSystemStatus (const Public::SystemStatus & m) override
   {
      if (trace)
         std::clog << "SystemStatus: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onInstrumentInfo (const Public::InstrumentInfo & m) override
   {
      if (trace)
         std::clog << "InstrumentInfo: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onInstrumentStatus (const Public::InstrumentStatus & m) override
   {
      if (trace)
         std::clog << "InstrumentStatus: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onIndexInfo (const Public::IndexInfo & m) override
   {
      if (trace)
         std::clog << "IndexInfo: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onIndexMemberInfo (const Public::IndexMemberInfo & m) override
   {
      if (trace)
         std::clog << "IndexMemberInfo: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onIndexUpdate (const Public::IndexUpdate & m) override
   {
      if (trace)
         std::clog << "IndexUpdate: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onTopOfBook (const Public::TopOfBook & m) override
   {
      if (trace)
         std::clog << "TopOfBook: " << m.toString () << std::endl;
      enc.send (m);
   }
   
   void
   onTradeStats (const Public::TradeStats & m) override
   {
      if (trace)
         std::clog << "TradeStats: " << m.toString () << std::endl;
      enc.send (m);
   }
   
   void
   onOrderInserted (const Public::OrderInserted & m) override
   {
      if (trace)
         std::clog << "OrderInserted: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onOrderDeleted (const Public::OrderDeleted & m) override
   {
      if (trace)
         std::clog << "OrderDeleted: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onOrderReduced (const Public::OrderReduced & m) override
   {
      if (trace)
         std::clog << "OrderReduced: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onOrderReplaced (const Public::OrderReplaced & m) override
   {
      if (trace)
         std::clog << "OrderReplaced: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onOrderExecuted (const Public::OrderExecuted & m) override
   {
      if (trace)
         std::clog << "OrderExecuted: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onInsertOrderReply (const Private::InsertOrderReply & m) override
   {
      if (trace)
         std::clog << "InsertOrderReply: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onDeleteOrderReply (const Private::DeleteOrderReply & m) override
   {
      if (trace)
         std::clog << "DeleteOrderReply: " << m.toString () << std::endl;
      enc.send (m);
   }
   
   void
   onReduceOrderReply (const Private::ReduceOrderReply & m) override
   {
      if (trace)
         std::clog << "ReduceOrderReply: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onReplaceOrderReply (const Private::ReplaceOrderReply & m) override
   {
      if (trace)
         std::clog << "ReplaceOrderReply: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onRejectOrderReply (const Private::RejectOrderReply & m) override
   {
      if (trace)
         std::clog << "RejectOrderReply: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onInsertOrder (const Private::InsertOrder & m) override
   {
      if (trace)
         std::clog << "InsertOrder: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onDeleteOrder (const Private::DeleteOrder & m) override
   {
      if (trace)
         std::clog << "DeleteOrder: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onReduceOrder (const Private::ReduceOrder & m) override
   {
      if (trace)
         std::clog << "ReduceOrder: " << m.toString () << std::endl;
      enc.send (m);
   }

   void
   onReplaceOrder (const Private::ReplaceOrder & m) override
   {
      if (trace)
         std::clog << "ReplaceOrder: " << m.toString () << std::endl;
      enc.send (m);
   }

   Encoder enc;
   bool trace;
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
   MyObserver o1;
// o1.enableTrace ();
   
   if (ac != 3)
      usage (av [0]);

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
      o1.setBinaryOutput ();

   if (binInput)
   {
      std::cerr << "Decoder: Binary..." << std::endl;
      ACT::BinDecoder d (o1);
      d.run ();
   }
   else
   {
      std::cerr << "Decoder: Text..." << std::endl;
      ACT::TextDecoder d (o1);
      d.run ();
   }
   return 0;
}
