//		ui512_stub_testing
// 
//		File:			ui512_stub_testing.cpp
//		Author:			John G.Lynch
//		Legal:			Copyright @2024, per MIT License below
//		Date:			October 29, 2025
//


#include "CppUnitTest.h"
#include "ui512_externs.h"
#include "ui512_unit_tests.h"

#include "intrin.h"
#include <chrono>
#include <cstring>
#include <format>
#include <sstream>
#include <string>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ui512_Unit_Tests
{
	TEST_CLASS( ui512_stub_testing )
	{
		struct div_u_Locals
		{
			alignas( 64 ) u64 currnumerator [ 16 ];		// scratch working copy of dividend( numerator ).could be 9 qwords, 16 declared for alignment
			u64 qdiv [ 16 ];							// scratch working copy of( trial ) qhat* divisor.could be 9 qwords, 16 declared for alignment
			u64 quotient [ 8 ];							// working copy of quotient
			u64	normdivisor [ 8 ];						// working copy of normalized divisor

			u64	nDiv;									// first qword of normalized divisor
			u64 nDiv1;									// second qword of normalized divisor
			u64	qHat;									// trial quotient
			u64	rHat;									// trial remainder
			u64 pad [ 4 ];
			u16	S_Idx;									// snapshot index for caller passed snapshot area
			u16	Ad_Cnt;

			u16	mMSB;									// indexes and dimensions of dividend ( numerator ) Note: dimensions are zero - based( 0 to 7 )
			u16	mDim;
			u16	mIdx;
			u16	mllimit;

			u16	nMSB;									// indexes and dimensions of divisor ( denominator )
			u16	nDim;
			u16	nIdx;
			u16	nllimit;

			u16	jDim;
			u16	jIdx;
			u16	jllimit;

			u16	normf;									// normalization factor ( bits to shift left )

			u16 filler [ 2 ];							// to get to 16 byte align for stack alloc( adjust as necessary )
			u64	RCXL;
			u64 	SaveSpace [ 3 ];
		};

		TEST_METHOD( ui512_01_Stub1_Dimensions )
		{
			{
				string test_message = string( "***\t\t\t" ) + "Stub Test 01" + "\t\t\t***\n\n";

				test_message += format( "Running div_stub1 with sample dividend(s) and divisor(s).\n" );
				test_message += format( "Given m/n, determine msb(s), dimensions, limits, indexes.\n" );
				test_message += format( "Normalize divisor, adjust dividend accordingly.\n" );
				test_message += format( "Set up for main division loop.\n" );
				test_message += format( "This first run displays relevant information from one run with fixed samples\n" );
				test_message += format( "Then remaining loop of tests: varying dimensions, and placements of MSB.\n\n" );

				div_u_Locals stub1_locals = {};
				div_u_Locals* local_ptr = &stub1_locals;
				_UI512( dividend ) = { 0,0,1,2,3,4,5,6 };
				_UI512( divisor ) = { 0,0,0,0,0,0, 2,3 };

				test_message += format( "Dividend: \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
					dividend [ 0 ], dividend [ 1 ], dividend [ 2 ], dividend [ 3 ],
					dividend [ 4 ], dividend [ 5 ], dividend [ 6 ], dividend [ 7 ] );

				test_message += format( "Divisor: \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
					divisor [ 0 ], divisor [ 1 ], divisor [ 2 ], divisor [ 3 ],
					divisor [ 4 ], divisor [ 5 ], divisor [ 6 ], divisor [ 7 ] );

				s16 retcode = div_stub1( ( const u64* ) local_ptr, nullptr, dividend, divisor );

				test_message += format( "div_stub1 return code: {:04x}\n\n", retcode );
				Assert::AreEqual( ( s16 ) 0x0a0a, retcode );

				test_message += format( "Dimensions :\nmMSB:{:d}\tmDim:{:d}\tmIdx:{:d}\tmlLimit:{:d}\nnMSB:{:d}\tnDim:{:d}\tnIdx:{:d}\tnlLimit:{:d}\n\n",
					local_ptr->mMSB, local_ptr->mDim, local_ptr->mIdx, local_ptr->mllimit,
					local_ptr->nMSB, local_ptr->nDim, local_ptr->nIdx, local_ptr->nllimit );
				Assert::AreEqual( ( u16 ) 382, local_ptr->mMSB );
				Assert::AreEqual( ( u16 ) 5, local_ptr->mDim );
				Assert::AreEqual( ( u16 ) 10, local_ptr->mIdx );
				Assert::AreEqual( ( u16 ) 10, local_ptr->mllimit );
				Assert::AreEqual( ( u16 ) 127, local_ptr->nMSB );
				Assert::AreEqual( ( u16 ) 1, local_ptr->nDim );
				Assert::AreEqual( ( u16 ) 6, local_ptr->nIdx );
				Assert::AreEqual( ( u16 ) 6, local_ptr->nllimit );

				test_message += format( "Normed Divisor (n): \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
					local_ptr->normdivisor [ 0 ], local_ptr->normdivisor [ 1 ], local_ptr->normdivisor [ 2 ], local_ptr->normdivisor [ 3 ],
					local_ptr->normdivisor [ 4 ], local_ptr->normdivisor [ 5 ], local_ptr->normdivisor [ 6 ], local_ptr->normdivisor [ 7 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 0 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 1 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 2 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 3 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 4 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 5 ] );
				// Note: normalized, shifted left until MSB (within word) is on
				Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->normdivisor [ 6 ] );
				Assert::AreEqual( ( u64 ) 0xc000000000000000, local_ptr->normdivisor [ 7 ] );

				test_message += format( "Curr Numerator (m) (adj for norm):\n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n",
					local_ptr->currnumerator [ 0 ], local_ptr->currnumerator [ 1 ], local_ptr->currnumerator [ 2 ], local_ptr->currnumerator [ 3 ],
					local_ptr->currnumerator [ 4 ], local_ptr->currnumerator [ 5 ], local_ptr->currnumerator [ 6 ], local_ptr->currnumerator [ 7 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 0 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 1 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 2 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 3 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 4 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 5 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 6 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 7 ] );

				test_message += format( "{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
					local_ptr->currnumerator [ 8 ], local_ptr->currnumerator [ 9 ], local_ptr->currnumerator [ 10 ], local_ptr->currnumerator [ 11 ],
					local_ptr->currnumerator [ 12 ], local_ptr->currnumerator [ 13 ], local_ptr->currnumerator [ 14 ], local_ptr->currnumerator [ 15 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 8 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 9 ] );
				Assert::AreEqual( ( u64 ) 0x4000000000000000, local_ptr->currnumerator [ 10 ] );
				Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->currnumerator [ 11 ] );
				Assert::AreEqual( ( u64 ) 0xc000000000000001, local_ptr->currnumerator [ 12 ] );
				Assert::AreEqual( ( u64 ) 0x0000000000000001, local_ptr->currnumerator [ 13 ] );
				Assert::AreEqual( ( u64 ) 0x4000000000000001, local_ptr->currnumerator [ 14 ] );
				Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->currnumerator [ 15 ] );

				Logger::WriteMessage( test_message.c_str( ) );
			};

			// Further tests with varying dimensions and MSB placements
			{
				u64 seed = 0;
				// first test set: varying divisor dimensions and MSB placements
				// fixed dividend for all of these tests
				_UI512( dividend ) = { 0x7fffffffffffffff,0x6fffffffffffffff, 0x5fffffffffffff, 0x4eeeeeeeeeeeeeee,
						0x3eeeeeeeeeeeeeee, 0xa77777777777777, 0xc444444444444444, 0x0000000055000000 };
				_UI512( divisor ) = { 0 };
				u16 emMSB = 510; // fixed for dividend above MSQWord 0 bit 62
				u16 emDim = 7; // fixed for dividend above, after normalization if qword added 
				u16 emIdx = 15 - emDim; // fixed for dividend above
				u16 emllimit = 15 - emDim; // fixed for dividend above
				u16 enMSB = 0;
				u16 enDim = 0;
				u16 enIdx = 7;
				u16 enllimit = 7;
				// check nMSB, if mod 64 == zero or one, will not cause normalization to add a word
				string test_message = string( "***\t\t\t" ) + "Stub Test 01 - Extended Divisor Variations" + "\t\t\t***\n\n";
				test_message += "\t\t\t\t Numbers are Actual (expected)\n";
				test_message += "Iter\tRetC\t  mMSB\t\tmDim\tmIdx\tmlLim\t  nMSB\t\tnDim\tnIdx\tnlLim\tNormF\tnDiv\n\n";
				Logger::WriteMessage( test_message.c_str( ) );
				u16 iteration = 0;
				u16 a_retcode = 0;
				u16 b_retcode = 0;
				for ( u16 inMSB = 0; inMSB < 512; inMSB++ )
				{
					iteration++;
					div_u_Locals stub1_locals = {};
					div_u_Locals* local_ptr = &stub1_locals;
					u16 enDim = inMSB >> 6;
					emMSB = 510; // fixed for dividend above MSQWord 0 bit 62
					emDim = 7; // fixed for dividend above, after normalization if qword added 
					emIdx = 15 - emDim; // fixed for dividend above
					emllimit = 15 - emDim; // fixed for dividend above
					zero_u( divisor );
					// set random divisor with given dimension and MSB
					u16 bits_to_set = inMSB + 1;
					for ( u16 i = 0; i <= enDim; i++ )
					{
						if ( bits_to_set >= 64 )
						{
							divisor [ 7 - i ] = 0x8000000000000000 | RandomU64( &seed );	// ensure MSB set
							bits_to_set -= 64;
						}
						else
						{
							divisor [ 7 - i ] = 0xffffffffffffffff >> ( 64 - bits_to_set );
							bits_to_set = 0;
						};
					};

					s16 nMSB = msb_u( ( const u64* ) divisor );

					u16 normed = 0;
					u16 emIdx = 0;
					u16 emllimit = 0;
					if ( nMSB == -1 )
					{
						normed = 0;
						emDim = 0;
						emIdx = 15;
						emllimit = 15;
					}
					else
					{
						Assert::AreEqual( inMSB, ( u16 ) nMSB, _MSGW( "Iteration " << iteration ) );
						normed = 63 - ( nMSB & 63 );
						emMSB += normed;
						emDim = emMSB >> 6;
						emIdx = 15 - emDim;
						emllimit = 15 - emDim;
						enMSB = nMSB + normed;
						enDim = enMSB >> 6;
						enIdx = 7 - enDim;
						enllimit = 7 - enDim;
					};
					// run stub
					s16 retcode = div_stub1( ( const u64* ) local_ptr, nullptr, dividend, divisor );

					string reasoncode = "";
					if ( retcode == ( s16 ) 0x0a0a )
					{
						reasoncode = "M / N"; // all good
						a_retcode++;
						string test_message = "";
						test_message += format( "{:04d}\t{:04x}\t{:d}({:d})\t{:d}({:d})\t{:d}({:d})\t{:d}",
							iteration, retcode,
							local_ptr->mMSB, emMSB, local_ptr->mDim, emDim, local_ptr->mIdx, emIdx, local_ptr->mllimit );
						u64 div1 = local_ptr->currnumerator [ emIdx ];
						u64 div2 = local_ptr->currnumerator [ emIdx + 1 ];
						test_message += format( "\t\t\t{:d}({:d})({:d}) \t{:d}({:d})\t{:d}({:d})\t{:d}\t\t{:d}({:d})\t{:016x} {:016x} {:016x}",
							local_ptr->nMSB, enMSB, nMSB, local_ptr->nDim, enDim, local_ptr->nIdx, enIdx, local_ptr->nllimit,
							local_ptr->normf, normed, local_ptr->nDiv, div1, div2 );

						bool okrange = ( iteration >= 64 && iteration <= 512 );
						Assert::IsTrue( okrange, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( emMSB, local_ptr->mMSB, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( emDim, local_ptr->mDim, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( emIdx, local_ptr->mIdx, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( emllimit, local_ptr->mllimit, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( enMSB, local_ptr->nMSB, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( enDim, local_ptr->nDim, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( enIdx, local_ptr->nIdx, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( enllimit, local_ptr->nllimit, _MSGW( "Iteration " << iteration ) );
						Assert::AreEqual( normed, local_ptr->normf, _MSGW( "Iteration " << iteration ) );

						test_message += '\t' + reasoncode + '\n';

						Logger::WriteMessage( test_message.c_str( ) );
					};
					if ( retcode == ( s16 ) 0x0b0b )
					{
						reasoncode = "M/1 N";
						b_retcode++;
						// one qword divisor
					};
					if ( retcode == ( s16 ) 0xffff )
					{
						// div by zero
					};
					if ( retcode == ( s16 ) 0x0909 )
					{
						// divisor > dividend
						reasoncode = "Div>Divd";
					};
				};
			}
		};

		TEST_METHOD( ui512_01_Stub2_MulSub )
		{
			const u16 s2_l_size = 20;
			div_u_Locals s2_l [ s2_l_size ] = { };
			div_u_Locals* local_ptr = &s2_l [ 0 ];
			_UI512( dividend ) = { 0,0,1,2,3,4,5,6 };
			_UI512( divisor ) = { 0,0,0,0,0,0, 2,3 };

			string test_message = string( "***\t\t\t" ) + "Stub Test 02" + "\t\t\t***\n\n";
			test_message += format( "Running div_stub2 with sample dividend(s) and divisor(s).\n" );
			test_message += format( "Take snapshots during main division loop\n" );
			test_message += format( "Each iteration calculates qhat and rhat, tests and adjusts if necessary\n" );
			test_message += format( "Then multiplies qhat times the normailed divisor, and subtracts it from the remaining numerator\n" );
			test_message += format( "The snap shot are at the begining of the loop, and one final one before exit.\n" );
			test_message += format( "Dividend: \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
				dividend [ 0 ], dividend [ 1 ], dividend [ 2 ], dividend [ 3 ],
				dividend [ 4 ], dividend [ 5 ], dividend [ 6 ], dividend [ 7 ] );

			test_message += format( "Divisor: \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
				divisor [ 0 ], divisor [ 1 ], divisor [ 2 ], divisor [ 3 ],
				divisor [ 4 ], divisor [ 5 ], divisor [ 6 ], divisor [ 7 ] );

			// run stub
			s16 retcode = div_stub2( ( const u64* ) local_ptr, nullptr, dividend, divisor );

			test_message += format( "Dimensions :\nmMSB:{:d}\tmDim:{:d}\tmIdx:{:d}\tmlLimit:{:d}\nnMSB:{:d}\tnDim:{:d}\tnIdx:{:d}\tnlLimit:{:d}\n\n",
				local_ptr->mMSB, local_ptr->mDim, local_ptr->mIdx, local_ptr->mllimit,
				local_ptr->nMSB, local_ptr->nDim, local_ptr->nIdx, local_ptr->nllimit );
			Assert::AreEqual( ( u16 ) 382, local_ptr->mMSB );
			Assert::AreEqual( ( u16 ) 5, local_ptr->mDim );
			Assert::AreEqual( ( u16 ) 10, local_ptr->mIdx );
			Assert::AreEqual( ( u16 ) 10, local_ptr->mllimit );
			Assert::AreEqual( ( u16 ) 127, local_ptr->nMSB );
			Assert::AreEqual( ( u16 ) 1, local_ptr->nDim );
			Assert::AreEqual( ( u16 ) 6, local_ptr->nIdx );
			Assert::AreEqual( ( u16 ) 6, local_ptr->nllimit );

			test_message += format( "Normed Divisor (n): \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
				local_ptr->normdivisor [ 0 ], local_ptr->normdivisor [ 1 ], local_ptr->normdivisor [ 2 ], local_ptr->normdivisor [ 3 ],
				local_ptr->normdivisor [ 4 ], local_ptr->normdivisor [ 5 ], local_ptr->normdivisor [ 6 ], local_ptr->normdivisor [ 7 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 0 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 1 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 2 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 3 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 4 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->normdivisor [ 5 ] );
			// Note: normalized, shifted left until MSB (within word) is on
			Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->normdivisor [ 6 ] );
			Assert::AreEqual( ( u64 ) 0xc000000000000000, local_ptr->normdivisor [ 7 ] );

			test_message += format( "Curr Numerator (m) (adj for norm):\n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n",
				local_ptr->currnumerator [ 0 ], local_ptr->currnumerator [ 1 ], local_ptr->currnumerator [ 2 ], local_ptr->currnumerator [ 3 ],
				local_ptr->currnumerator [ 4 ], local_ptr->currnumerator [ 5 ], local_ptr->currnumerator [ 6 ], local_ptr->currnumerator [ 7 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 0 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 1 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 2 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 3 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 4 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 5 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 6 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 7 ] );

			test_message += format( "{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
				local_ptr->currnumerator [ 8 ], local_ptr->currnumerator [ 9 ], local_ptr->currnumerator [ 10 ], local_ptr->currnumerator [ 11 ],
				local_ptr->currnumerator [ 12 ], local_ptr->currnumerator [ 13 ], local_ptr->currnumerator [ 14 ], local_ptr->currnumerator [ 15 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 8 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000000, local_ptr->currnumerator [ 9 ] );
			Assert::AreEqual( ( u64 ) 0x4000000000000000, local_ptr->currnumerator [ 10 ] );
			Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->currnumerator [ 11 ] );
			Assert::AreEqual( ( u64 ) 0xc000000000000001, local_ptr->currnumerator [ 12 ] );
			Assert::AreEqual( ( u64 ) 0x0000000000000001, local_ptr->currnumerator [ 13 ] );
			Assert::AreEqual( ( u64 ) 0x4000000000000001, local_ptr->currnumerator [ 14 ] );
			Assert::AreEqual( ( u64 ) 0x8000000000000000, local_ptr->currnumerator [ 15 ] );

			test_message += format( "div_stub1 return code: {:04x}\n\n", retcode );
			switch ( retcode )
			{
			case 0x0f0f:		// divide by zero
				test_message += format( "Returned signalling divide by zero\n\n" );
				break;
			case 0x0707:		// too many adjustments
				test_message += format( "Returned signalling too many adjustments\n\n" );
				break;
			case 0x0e0e:		// divide by one
				test_message += format( "Returned signalling divide by one\n\n" );
				break;
			case 0x0909:		// divisor > dividend
				test_message += format( "Returned signalling divisor > dividend\n\n" );
				break;
			default:			// all good
				test_message += format( "Snapshot returned count: {:4d}\n\n", retcode );
				for ( s16 i = 0; i < retcode; i++ )
				{
					local_ptr = &s2_l [ i ];
					test_message += format( "Snapshot {:02d}:\n", i + 1 );
					test_message += format( "qHat:{:016x}\trHat:{:016x}\n", local_ptr->qHat, local_ptr->rHat );
					test_message += format( "Curr Numerator (m) (adj for norm):\n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n",
						local_ptr->currnumerator [ 0 ], local_ptr->currnumerator [ 1 ], local_ptr->currnumerator [ 2 ], local_ptr->currnumerator [ 3 ],
						local_ptr->currnumerator [ 4 ], local_ptr->currnumerator [ 5 ], local_ptr->currnumerator [ 6 ], local_ptr->currnumerator [ 7 ] );
					test_message += format( "{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
						local_ptr->currnumerator [ 8 ], local_ptr->currnumerator [ 9 ], local_ptr->currnumerator [ 10 ], local_ptr->currnumerator [ 11 ],
						local_ptr->currnumerator [ 12 ], local_ptr->currnumerator [ 13 ], local_ptr->currnumerator [ 14 ], local_ptr->currnumerator [ 15 ] );
					test_message += format( "Q Div:\n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n",
						local_ptr->qdiv [ 0 ], local_ptr->qdiv [ 1 ], local_ptr->qdiv [ 2 ], local_ptr->qdiv [ 3 ],
						local_ptr->qdiv [ 4 ], local_ptr->qdiv [ 5 ], local_ptr->qdiv [ 6 ], local_ptr->qdiv [ 7 ] );
					test_message += format( "{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
						local_ptr->qdiv [ 8 ], local_ptr->qdiv [ 9 ], local_ptr->qdiv [ 10 ], local_ptr->qdiv [ 11 ],
						local_ptr->qdiv [ 12 ], local_ptr->qdiv [ 13 ], local_ptr->qdiv [ 14 ], local_ptr->qdiv [ 15 ] );
					test_message += format( "Quotient: \n{:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x} {:016x}\n\n",
						local_ptr->quotient [ 0 ], local_ptr->quotient [ 1 ], local_ptr->quotient [ 2 ], local_ptr->quotient [ 3 ],
						local_ptr->quotient [ 4 ], local_ptr->quotient [ 5 ], local_ptr->quotient [ 6 ], local_ptr->quotient [ 7 ] );
				};

				break;
			};

			//test_message += format( "Samples run:\t\t\t\t{:9d}\n", stat->timing_count );
			//test_message += format( "Total target function (including c calling set - up) execution cycles :{:10.0f}\n", stat->total );
			//test_message += format( "Average clock cycles per call: \t{:6.2f}\n", stat->mean );
			//test_message += format( "Minimum in \t\t\t\t\t\t{:6.0f}\n", stat->min );
			//test_message += format( "Maximum in \t\t\t\t\t\t{:6.0f}\n", stat->max );
			//test_message += format( "Sample Variance: \t\t\t{:10.3f}\n", stat->sample_variance );
			//test_message += format( "Standard Deviation :\t \t{:9.3f}\n", stat->stddev );
			//test_message += format( "Coefficient of Variation: \t{:10.2f}\n\n", stat->coefficient_of_variation );

			Logger::WriteMessage( test_message.c_str( ) );
		};

		TEST_METHOD( ui512_01_Stub3 )
		{
			string test_message = string( "***\t\t\t" ) + "Stub Test 03" + "\t\t\t***\n\n";
			//test_message += format( "Samples run:\t\t\t\t{:9d}\n", stat->timing_count );
			//test_message += format( "Total target function (including c calling set - up) execution cycles :{:10.0f}\n", stat->total );
			//test_message += format( "Average clock cycles per call: \t{:6.2f}\n", stat->mean );
			//test_message += format( "Minimum in \t\t\t\t\t\t{:6.0f}\n", stat->min );
			//test_message += format( "Maximum in \t\t\t\t\t\t{:6.0f}\n", stat->max );
			//test_message += format( "Sample Variance: \t\t\t{:10.3f}\n", stat->sample_variance );
			//test_message += format( "Standard Deviation :\t \t{:9.3f}\n", stat->stddev );
			//test_message += format( "Coefficient of Variation: \t{:10.2f}\n\n", stat->coefficient_of_variation );

			Logger::WriteMessage( test_message.c_str( ) );
		};

		TEST_METHOD( ui512_01_Stub4 )
		{
			string test_message = string( "***\t\t\t" ) + "Stub Test 04" + "\t\t\t***\n\n";
			//test_message += format( "Samples run:\t\t\t\t{:9d}\n", stat->timing_count );
			//test_message += format( "Total target function (including c calling set - up) execution cycles :{:10.0f}\n", stat->total );
			//test_message += format( "Average clock cycles per call: \t{:6.2f}\n", stat->mean );
			//test_message += format( "Minimum in \t\t\t\t\t\t{:6.0f}\n", stat->min );
			//test_message += format( "Maximum in \t\t\t\t\t\t{:6.0f}\n", stat->max );
			//test_message += format( "Sample Variance: \t\t\t{:10.3f}\n", stat->sample_variance );
			//test_message += format( "Standard Deviation :\t \t{:9.3f}\n", stat->stddev );
			//test_message += format( "Coefficient of Variation: \t{:10.2f}\n\n", stat->coefficient_of_variation );

			Logger::WriteMessage( test_message.c_str( ) );
		};

	};	// test_class
};	// namespace