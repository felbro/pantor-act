#include "Enc.h"
#include <iomanip>
using namespace ACT;

void
Encoder::encodeBin (u16 v) const
{
   if (trace)
      std::clog << "encodeBin u16=" << v << std::endl;
   ::write (1, & v, 2);
}

void
Encoder::encodeBin (u32 v) const
{
   if (trace)
      std::clog << "encodeBin u32=" << v << std::endl;
   ::write (1, & v, 4);
}

void
Encoder::encodeBin (u64 v) const
{
   if (trace)
      std::clog << "encodeBin u64=" << v << std::endl;
   ::write (1, & v, 8);
}

void
Encoder::encodeBin (i64 v) const
{
   if (trace)
      std::clog << "encodeBin i64=" << v << std::endl;
   ::write (1, & v, 8);
}

void
Encoder::encodeBin (const std::string & s, u32 size) const
{
   if (trace)
      std::clog << "encodeBin '" << s << "'" << std::endl;

   size_t n = s.length ();

   if (n > size)
   {
      std::cerr << "Encoder: string too long: " << s << std::endl;
      abort ();
   }

   ::write (1, s.data (), n);

   if (size > n)
   {
      u8 padchars [] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
      u32 npad = size - n;
      write (1, & padchars, npad);
   }
}

template <typename T> void
Encoder::encodeText (const T & v, bool last) const
{
   std::cout <<  v;

   if (last)
      std::cout << std::endl;
   else
//    std::cout << "\t";
      std::cout << "|";
}

void
Encoder::encodePriceText (const u64 v, bool last) const
{
	u64 first = v/10000;
	u64 second = v % 10000;
	
	std::cout << first << "." << second;
	
	if (last)
		std::cout << std::endl;
	else
		std::cout << "|";

}

static inline double
toFloat (const u64 v)
{
   double d = static_cast<double> (v);
   d /= 10000.0;
   return d;
}

void
Encoder::send (const Private::InsertOrder & m)
{
   if (trace)
      std::clog << "sending InsertOrder: " << m.toString () << std::endl;

   u32 tif = static_cast<u32> (m.tif);
   u32 side = static_cast<u32> (m.side);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
      encodeBin (side);
      encodeBin (tif);
   }
   else
   {
      encodeText (m.type ());
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity);
      encodeText (side);
      encodeText (tif, true);
   }
}

void
Encoder::send (const Private::DeleteOrder & m)
{
   if (trace)
      std::clog << "sending DeleteOrder: " << m.toString () << std::endl;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
   }
   else
   {
      encodeText (m.type ());
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId, true);
   }
}

void
Encoder::send (const Private::ReduceOrder & m)
{
   if (trace)
      std::clog << "sending ReduceOrder: " << m.toString () << std::endl;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Private::ReplaceOrder & m)
{
   if (trace)
      std::clog << "sending ReplaceOrder: " << m.toString () << std::endl;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (m.newClientOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodeText (m.newClientOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity, true);
   }
}


void
Encoder::send (const Private::InsertOrderReply & m)
{
   if (trace)
      std::clog << "sending InsertOrderReply: " << m.toString () << std::endl;

   ++ seqNo;

   u32 side = static_cast<u32> (m.side);
   u32 tif = static_cast<u32> (m.tif);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (m.serverOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
      encodeBin (side);
      encodeBin (tif);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodeText (m.serverOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity);
      encodeText (side);
      encodeText (tif, true);
   }
}

void
Encoder::send (const Private::DeleteOrderReply & m)
{
   if (trace)
      std::clog << "sending DeleteOrderReply: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId, true);
   }
}

void
Encoder::send (const Private::ReduceOrderReply & m)
{
   if (trace)
      std::clog << "sending ReduceOrderReply: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Private::ReplaceOrderReply & m)
{
   if (trace)
      std::clog << "sending ReplaceOrderReply: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.oldClientOrderId);
      encodeBin (m.newClientOrderId);
      encodeBin (m.serverOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.oldClientOrderId);
      encodeText (m.newClientOrderId);
      encodeText (m.serverOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Private::RejectOrderReply & m)
{
   if (trace)
      std::clog << "sending RejectOrderReply: " << m.toString () << std::endl;

   ++ seqNo;

   u32 reason = static_cast<u32> (m.reason);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.clientId);
      encodeBin (m.clientOrderId);
      encodeBin (reason);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.clientId);
      encodeText (m.clientOrderId);
      encodeText (reason, true);
   }
}

void
Encoder::send (const Public::SystemStatus & m)
{
   if (trace)
      std::clog << "sending SystemStatus: " << m.toString () << std::endl;

   ++ seqNo;

   u32 status = static_cast<u32> (m.open);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (status);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (status, true);
   }
}

void
Encoder::send (const Public::InstrumentInfo & m)
{
   if (trace)
      std::clog << "sending InstrumentInfo: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.name, 16);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.name, true);
   }
}

void
Encoder::send (const Public::InstrumentStatus & m)
{
   if (trace)
      std::clog << "sending InstrumentStatus: " << m.toString () << std::endl;

   ++ seqNo;

   u32 status = static_cast<u32> (m.active);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (status);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (status, true);
   }
}

void
Encoder::send (const Public::IndexInfo & m)
{
   if (trace)
      std::clog << "sending IndexInfo: " << m.toString () << std::endl;

   ++ seqNo;

   u32 priceType = static_cast<u32> (m.priceType);
   u32 status = static_cast<u32> (m.active);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.indexId);
      encodeBin (m.value);
      encodeBin (priceType);
      encodeBin (status);
      encodeBin (m.name, 16);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.indexId);
      encodePriceText (m.value);
      encodeText (priceType);
      encodeText (status);
      encodeText (m.name, true);
   }
}

void
Encoder::send (const Public::IndexMemberInfo & m)
{
   if (trace)
      std::clog << "sending IndexMemberInfo: " << m.toString () << std::endl;

   ++ seqNo;

   u32 status = static_cast<u32> (m.included);

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.indexId);
      encodeBin (m.instrumentId);
      encodeBin (m.prevShares);
      encodeBin (m.currShares);
      encodeBin (m.prevPrice);
      encodeBin (m.issueAmount);
      encodeBin (m.dividend);
      encodeBin (status);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.indexId);
      encodeText (m.instrumentId);
      encodeText (m.prevShares);
      encodeText (m.currShares);
      encodePriceText (m.prevPrice);
      encodeText (m.issueAmount);
      encodePriceText (m.dividend);
      encodeText (status, true);
   }
}

void
Encoder::send (const Public::IndexUpdate & m)
{
   if (trace)
      std::clog << "sending IndexUpdate: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.indexId);
      encodeBin (m.value);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.indexId);
      encodePriceText (m.value, true);
   }
}

void
Encoder::send (const Public::OrderInserted & m)
{
   if (trace)
      std::clog << "sending OrderInserted: " << m.toString () << std::endl;

   ++ seqNo;

   u32 side = static_cast<u32> (m.side);
   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.serverOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
      encodeBin (side);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.serverOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity);
      encodeText (side, true);
   }
}

void
Encoder::send (const Public::OrderDeleted & m)
{
   if (trace)
      std::clog << "sending OrderDeleted: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.serverOrderId);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.serverOrderId, true);
   }
}

void
Encoder::send (const Public::OrderReduced & m)
{
   if (trace)
      std::clog << "sending OrderReduced: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.serverOrderId);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.serverOrderId);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Public::OrderReplaced & m)
{
   if (trace)
      std::clog << "sending OrderReplaced: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.serverOrderId);
      encodeBin (m.newServerOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.serverOrderId);
      encodeText (m.newServerOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Public::OrderExecuted & m)
{
   if (trace)
      std::clog << "sending OrderExecuted: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.serverOrderId);
      encodeBin (m.price);
      encodeBin (m.quantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodeText (m.serverOrderId);
      encodePriceText (m.price);
      encodeText (m.quantity, true);
   }
}

void
Encoder::send (const Public::TopOfBook & m)
{
   if (trace)
      std::clog << "sending TopOfBook: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.bidPrice);
      encodeBin (m.bidQuantity);
      encodeBin (m.askPrice);
      encodeBin (m.askQuantity);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodePriceText (m.bidPrice);
      encodeText (m.bidQuantity);
      encodePriceText (m.askPrice);
      encodeText (m.askQuantity, true);
   }
}

void
Encoder::send (const Public::TradeStats & m)
{
   if (trace)
      std::clog << "sending TradeStats: " << m.toString () << std::endl;

   ++ seqNo;

   if (binary)
   {
      encodeBin (m.size ());
      encodeBin (m.type ());
      encodeBin (seqNo);
      encodeBin (m.msgTsp);
      encodeBin (m.instrumentId);
      encodeBin (m.lastPrice);
      encodeBin (m.lastQuantity);
      encodeBin (m.vwapPrice);
   }
   else
   {
      encodeText (m.type ());
      encodeText (seqNo);
      encodeText (m.msgTsp);
      encodeText (m.instrumentId);
      encodePriceText (m.lastPrice);
      encodeText (m.lastQuantity);
      encodePriceText (m.vwapPrice, true);
   }
}
