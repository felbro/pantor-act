// -*- C++ -*-
// 

#pragma once
#include "base.h"

namespace ACT
{
   //
   // enums:
   enum class Side
   {
      Buy = 0,
      Sell = 1
   };

   enum class PriceType
   {
      TradedPrice = 1,
      VWAP = 2
   };

   enum class TimeInForce
   {
      IOC = 1,
      GTC
   };

   enum class RejectReason
   {
      InvalidInstrument = 1,
      InvalidPrice = 2,
      InvalidQuantity = 3,
      InvalidOrderId = 4,
	  InvalidNotionalValue = 5,
	  NotionalLimitExceeded = 6
   };

   // common msg header
   struct MsgBase
   {
      MsgBase () :
         msgSize (0),
         msgType (0),
         msgSeqNo (0),
         msgTsp (0) { }

      virtual ~MsgBase () { }
   
      std::string toString () const;

      u16 msgSize;
      u16 msgType;
      u32 msgSeqNo;
      u64 msgTsp;
   };


   namespace Public
   {
      struct SystemStatus : public MsgBase
      {
         SystemStatus () :
            open (false) { }

         static u16 size () { return 20; }
         static u16 type () { return 1; }
         std::string toString () const;
         
         bool open;
      };

      struct InstrumentInfo : public MsgBase
      {
         InstrumentInfo () :
            instrumentId (0) { }

         static u16 size () { return 36; }
         static u16 type () { return 2; }
         std::string toString () const;

         u32 instrumentId;
         std::string name;
      };

      struct InstrumentStatus : public MsgBase
      {
         InstrumentStatus () :
            instrumentId (0),
            active (false) { }

         static u16 size () { return 24; }
         static u16 type () { return 3; }
         std::string toString () const;

         u32 instrumentId;
         bool active;
      };

      struct IndexInfo : public MsgBase
      {
         IndexInfo () :
            indexId (0),
            value (0),
            priceType (PriceType::TradedPrice),
            active (true) { }
      
         static u16 size () { return 52; }
         static u16 type () { return 4; }
         std::string toString () const;

         u32 indexId;
         u64 value;
         PriceType priceType;
         bool active;
         std::string name;
      };

      struct IndexMemberInfo : public MsgBase
      {
         IndexMemberInfo () :
            indexId (0),
            instrumentId (0),
            prevShares (0),
            currShares (0),
            prevPrice (0),
            issueAmount (0),
            dividend (0),
            included (false) { }

         static u16 size () { return 68; }
         static u16 type () { return 5; }
         std::string toString () const;
         
         u32 indexId;
         u32 instrumentId;
         u64 prevShares;
         u64 currShares;
         u64 prevPrice;
         u64 issueAmount;
         u64 dividend;
         bool included;
      };

      struct IndexUpdate : public MsgBase
      {
         IndexUpdate () :
            indexId (0),
            value (0) { }

         static u16 size () { return 28; }
         static u16 type () { return 6; }
         std::string toString () const;

         u32 indexId;
         u64 value;
      };

      struct OrderInserted : public MsgBase
      {
         OrderInserted () :
            instrumentId (0),
            serverOrderId (0),
            price (0),
            quantity (0),
            side (Side::Buy) { }

         static u16 size () { return 48; }
         static u16 type () { return 7; }
         std::string toString () const;

         u32 instrumentId;
         u64 serverOrderId;
         u64 price;
         u64 quantity;
         Side side;
      };

      struct OrderDeleted : public MsgBase
      {
         OrderDeleted () :
            instrumentId (0),
            serverOrderId (0) { }

         static u16 size () { return 28; }
         static u16 type () { return 8; }
         std::string toString () const;
         
         u32 instrumentId;
         u64 serverOrderId;
      };

      struct OrderReduced : public MsgBase
      {
         OrderReduced () :
            instrumentId (0),
            serverOrderId (0),
            quantity (0) { }

         static u16 size () { return 36; }
         static u16 type () { return 9; }
         std::string toString () const;

         u32 instrumentId;
         u64 serverOrderId;
         u64 quantity;
      };

      struct OrderReplaced : public MsgBase
      {
         OrderReplaced () :
            instrumentId (0),
            serverOrderId (0),
            newServerOrderId (0),
            price (0),
            quantity (0) { }

         static u16 size () { return 52; }
         static u16 type () { return 10; }
         std::string toString () const;

         u32 instrumentId;
         u64 serverOrderId;
         u64 newServerOrderId;
         u64 price;
         u64 quantity;
      };

      struct OrderExecuted : public MsgBase
      {
         OrderExecuted () :
            instrumentId (0),
            serverOrderId (0),
            price (0),
            quantity (0) { }

         static u16 size () { return 44; }
         static u16 type () { return 11; }
         std::string toString () const;

         u32 instrumentId;
         u64 serverOrderId;
         u64 price;
         u64 quantity;
      };

      struct TopOfBook : public MsgBase
      {
         TopOfBook () :
            instrumentId (0),
            bidPrice (0),
            bidQuantity (0),
            askPrice (0),
            askQuantity (0) { }

         static u16 size () { return 52; }
         static u16 type () { return 12; }
         std::string toString () const;

         u32 instrumentId;
         u64 bidPrice;
         u64 bidQuantity;
         u64 askPrice;
         u64 askQuantity;
      };

      struct TradeStats : public MsgBase
      {
         TradeStats () :
            instrumentId (0),
            lastPrice (0),
            lastQuantity (0),
            vwapPrice (0) { }

         static u16 size () { return 44; }
         static u16 type () { return 13; }
         std::string toString () const;

         u32 instrumentId;
         u64 lastPrice;
         u64 lastQuantity;
         u64 vwapPrice;
      };
   }

   namespace Private
   {
      struct InsertOrder : public MsgBase
      {
         InsertOrder () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            price (0),
            quantity (0),
            side (Side::Buy),
            tif (TimeInForce::IOC) { }

         static u16 size () { return 44; }
         static u16 type () { return 21; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         u64 price;
         u64 quantity;
         Side side;
         TimeInForce tif;
      };

      struct DeleteOrder : public MsgBase
      {
         DeleteOrder () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0) { }

         static u16 size () { return 20; }
         static u16 type () { return 22; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
      };

      struct ReduceOrder : public MsgBase
      {
         ReduceOrder () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            quantity (0) { }

         static u16 size () { return 28; }
         static u16 type () { return 23; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         u64 quantity;
      };

      struct ReplaceOrder : public MsgBase
      {
         ReplaceOrder () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            newClientOrderId (0),
            price (0),
            quantity (0) { }

         static u16 size () { return 44; }
         static u16 type () { return 24; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         u64 newClientOrderId;
         u64 price;
         i64 quantity;
      };

      struct InsertOrderReply : MsgBase
      {
         InsertOrderReply () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            serverOrderId (0),
            price (0),
            quantity (0),
            side (Side::Buy),
            tif (TimeInForce::IOC) { }

         static u16 size () { return 64; }
         static u16 type () { return 25; }
         std::string toString () const;
         
         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         u64 serverOrderId;
         u64 price;
         u64 quantity;
         Side side;
         TimeInForce tif;
      };
   
      struct DeleteOrderReply : MsgBase
      {
         DeleteOrderReply () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0) { }

         static u16 size () { return 32; }
         static u16 type () { return 26; }
         std::string toString () const;
      
         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
      };
   
      struct ReduceOrderReply : MsgBase
      {
         ReduceOrderReply () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            quantity (0) { }

         static u16 size () { return 40; }
         static u16 type () { return 27; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         u64 quantity;
      };
   
      struct ReplaceOrderReply : MsgBase
      {
         ReplaceOrderReply () :
            instrumentId (0),
            clientId (0),
            oldClientOrderId (0),
            newClientOrderId (0),
            serverOrderId (0),
            price (0),
            quantity (0) { }

         static u16 size () { return 64; }
         static u16 type () { return 28; }
         std::string toString () const;
         
         u32 instrumentId;
         u32 clientId;
         u64 oldClientOrderId;
         u64 newClientOrderId;
         u64 serverOrderId;
         u64 price;
         u64 quantity;
      };
   
      struct RejectOrderReply : MsgBase
      {
         RejectOrderReply () :
            instrumentId (0),
            clientId (0),
            clientOrderId (0),
            reason (RejectReason::InvalidPrice) { }

         static u16 size () { return 36; }
         static u16 type () { return 29; }
         std::string toString () const;

         u32 instrumentId;
         u32 clientId;
         u64 clientOrderId;
         RejectReason reason;
      };
   }

}
