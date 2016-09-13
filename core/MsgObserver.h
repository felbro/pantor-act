// -*- C++ -*-
// 

#pragma once
#include "Msgs.h"

namespace ACT
{
   struct MsgObserver
   {
      virtual ~MsgObserver () { }

      virtual void onSystemStatus (const Public::SystemStatus &) = 0;
      virtual void onInstrumentInfo (const Public::InstrumentInfo &) = 0;
      virtual void onInstrumentStatus (const Public::InstrumentStatus &) = 0;
      virtual void onIndexInfo (const Public::IndexInfo &) = 0;
      virtual void onIndexMemberInfo (const Public::IndexMemberInfo &) = 0;
      virtual void onIndexUpdate (const Public::IndexUpdate &) = 0;
      virtual void onTopOfBook (const Public::TopOfBook &) = 0;
      virtual void onTradeStats (const Public::TradeStats &) = 0;

      virtual void onOrderInserted (const Public::OrderInserted &) = 0;
      virtual void onOrderDeleted (const Public::OrderDeleted &) = 0;
      virtual void onOrderReduced (const Public::OrderReduced &) = 0;
      virtual void onOrderReplaced (const Public::OrderReplaced &) = 0;
      virtual void onOrderExecuted (const Public::OrderExecuted &) = 0;

      virtual void onInsertOrderReply (const Private::InsertOrderReply &) = 0;
      virtual void onDeleteOrderReply (const Private::DeleteOrderReply &) = 0;
      virtual void onReduceOrderReply (const Private::ReduceOrderReply &) = 0;
      virtual void onReplaceOrderReply (const Private::ReplaceOrderReply &) = 0;
      virtual void onRejectOrderReply (const Private::RejectOrderReply &) = 0;

      virtual void onInsertOrder (const Private::InsertOrder &) = 0;
      virtual void onDeleteOrder (const Private::DeleteOrder &) = 0;
      virtual void onReduceOrder (const Private::ReduceOrder &) = 0;
      virtual void onReplaceOrder (const Private::ReplaceOrder &) = 0;
   };
}
