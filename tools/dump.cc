//
// Simple message dumper
//

#include "Dec.h"
#include "Enc.h"

using namespace ACT;

struct Dumper : public MsgObserver
{
   void
   onSystemStatus (const Public::SystemStatus & m) override
   {
      std::clog << m.MsgBase::toString () << ": SystemStatus: "
                << m.toString () << std::endl;
   }

   void
   onInstrumentInfo (const Public::InstrumentInfo & m) override
   {
      std::clog << m.MsgBase::toString () << ": InstrumentInfo: "
                << m.toString () << std::endl;
   }

   void
   onInstrumentStatus (const Public::InstrumentStatus & m) override
   {
      std::clog << m.MsgBase::toString () << ": InstrumentStatus: "
                << m.toString () << std::endl;
   }

   void
   onIndexInfo (const Public::IndexInfo & m) override
   {
      std::clog << m.MsgBase::toString () << ": IndexInfo: "
                << m.toString () << std::endl;
   }

   void
   onIndexMemberInfo (const Public::IndexMemberInfo & m) override
   {
      std::clog << m.MsgBase::toString () << ": IndexMemberInfo: "
                << m.toString () << std::endl;
   }

   void
   onIndexUpdate (const Public::IndexUpdate & m) override
   {
      std::clog << m.MsgBase::toString () << ": IndexUpdate: "
                << m.toString () << std::endl;
   }

   void
   onTopOfBook (const Public::TopOfBook & m) override
   {
      std::clog << m.MsgBase::toString () << ": TopOfBook: "
                << m.toString () << std::endl;
   }
   
   void
   onTradeStats (const Public::TradeStats & m) override
   {
      std::clog << m.MsgBase::toString () << ": TradeStats: "
                << m.toString () << std::endl;
   }
   
   void
   onOrderInserted (const Public::OrderInserted & m) override
   {
      std::clog << m.MsgBase::toString () << ": OrderInserted: "
                << m.toString () << std::endl;
   }

   void
   onOrderDeleted (const Public::OrderDeleted & m) override
   {
      std::clog << m.MsgBase::toString () << ": OrderDeleted: "
                << m.toString () << std::endl;
   }

   void
   onOrderReduced (const Public::OrderReduced & m) override
   {
      std::clog << m.MsgBase::toString () << ": OrderReduced: "
                << m.toString () << std::endl;
   }

   void
   onOrderReplaced (const Public::OrderReplaced & m) override
   {
      std::clog << m.MsgBase::toString () << ": OrderReplaced: "
                << m.toString () << std::endl;
   }

   void
   onOrderExecuted (const Public::OrderExecuted & m) override
   {
      std::clog << m.MsgBase::toString () << ": OrderExecuted: "
                << m.toString () << std::endl;
   }

   // private messages:
   void
   onInsertOrderReply (const Private::InsertOrderReply & m) override
   {
      std::clog << m.MsgBase::toString () << ": InsertOrderReply: "
                << m.toString () << std::endl;
   }

   void
   onDeleteOrderReply (const Private::DeleteOrderReply & m) override
   {
      std::clog << m.MsgBase::toString () << ": DeleteOrderReply: "
                << m.toString () << std::endl;
   }
   
   void
   onReduceOrderReply (const Private::ReduceOrderReply & m) override
   {
      std::clog << m.MsgBase::toString () << ": ReduceOrderReply: "
                << m.toString () << std::endl;
   }

   void
   onReplaceOrderReply (const Private::ReplaceOrderReply & m) override
   {
      std::clog << m.MsgBase::toString () << ": ReplaceOrderReply: "
                << m.toString () << std::endl;
   }

   void
   onRejectOrderReply (const Private::RejectOrderReply & m) override
   {
      std::clog << m.MsgBase::toString () << ": RejectOrderReply: "
                << m.toString () << std::endl;
   }

   void
   onInsertOrder (const Private::InsertOrder & m) override
   {
      std::clog << m.MsgBase::toString () << ": InsertOrder: "
                << m.toString () << std::endl;
   }
   
   void
   onDeleteOrder (const Private::DeleteOrder & m) override
   {
      std::clog << m.MsgBase::toString () << ": DeleteOrder: "
                << m.toString () << std::endl;
   }
   
   void
   onReduceOrder (const Private::ReduceOrder & m) override
   {
      std::clog << m.MsgBase::toString () << ": ReduceOrder: "
                << m.toString () << std::endl;
   }
   
   void
   onReplaceOrder (const Private::ReplaceOrder & m) override
   {
      std::clog << m.MsgBase::toString () << ": ReplaceOrder: "
                << m.toString () << std::endl;
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
   Dumper dumper;
   
   if (ac != 2)
      usage (av [0]);
   
   if (* av [1] == 't')
   {
      std::cerr << "Decoder: Text..." << std::endl;
      ACT::TextDecoder d (dumper);
      d.run ();
   }
   else if (* av [1] == 'b')
   {
      std::cerr << "Decoder: Bin..." << std::endl;
      ACT::BinDecoder d (dumper);
      d.run ();
   }
   else
      usage (av [0]);

   return 0;
}
