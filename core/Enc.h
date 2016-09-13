// -*- C++ -*-
// 

#pragma once

#include "base.h"
#include "Msgs.h"

namespace ACT
{
   class Encoder
   {
   public:
      explicit Encoder (bool bin = false) :
         binary (bin),
         trace (false),
         seqNo (0) { }

      void setBinary () { binary = true; }
      void enableTrace () { trace = true; }
      
      void send (const Private::InsertOrder &);
      void send (const Private::DeleteOrder &);
      void send (const Private::ReduceOrder &);
      void send (const Private::ReplaceOrder &);

      void send (const Private::InsertOrderReply &);
      void send (const Private::DeleteOrderReply &);
      void send (const Private::ReduceOrderReply &);
      void send (const Private::ReplaceOrderReply &);
      void send (const Private::RejectOrderReply &);

      void send (const Public::SystemStatus &);
      void send (const Public::InstrumentInfo &);
      void send (const Public::InstrumentStatus &);
      void send (const Public::IndexInfo &);
      void send (const Public::IndexMemberInfo &);
      void send (const Public::IndexUpdate &);
      void send (const Public::OrderInserted &);
      void send (const Public::OrderDeleted &);
      void send (const Public::OrderReduced &);
      void send (const Public::OrderReplaced &);
      void send (const Public::OrderExecuted &);
      void send (const Public::TopOfBook &);
      void send (const Public::TradeStats &);

   private:

      // field encoding, binary
      void encodeBin (u16) const;
      void encodeBin (u32) const;
      void encodeBin (u64) const;
      void encodeBin (i64) const;
      void encodeBin (const std::string &, u32 len) const;

      // ... text
      template <typename T>
      void encodeText (const T &, bool last = false) const;

      //
      bool binary;
      bool trace;
      u32 seqNo;
   };
   
}
