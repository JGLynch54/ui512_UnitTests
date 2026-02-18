//		ui512_unit_tests_division
// 
//		File:			ui512_unit_tests_division.cpp
//		Author:			John G.Lynch
//		Legal:			Copyright @2026, per MIT License below
//		Date:			November 24, 2025 (file creation)
//
//		ui512 is a small project to provide basic operations for a variable type of unsigned 512 bit integer.
//		The basic operations : zero, copy, compare, add, subtract.
//		Other optional modules provide bit ops and multiply / divide.
//		It is written in assembly language, using the MASM ( ml64 ) assembler provided as an option within Visual Studio.
//		( currently using VS Community 2022 17.14.10)
//		It provides external signatures that allow linkage to C and C++ programs,
//		where a shell / wrapper could encapsulate the methods as part of an object.
//		It has assembly time options directing the use of Intel processor extensions : AVX4, AVX2, SIMD, or none :
//		( Z ( 512 ), Y ( 256 ), or X ( 128 ) registers, or regular Q ( 64bit ) ).
//		If processor extensions are used, the caller must align the variables declared and passed
//		on the appropriate byte boundary ( e.g. alignas 64 for 512 )
//		These modules (in total) are very light-weight ( less than 10K bytes ) and relatively fast,
//		but is not intended for all processor types or all environments.
// 
//		Intended use cases :
//			1.) a "sum of primes" for primes up to 2 ^ 48.
//			2.) elliptical curve cryptography(ECC)
//
//		This sub - project: ui512_unit_tests_division, is a unit test project that invokes each of the routines in the ui512a assembly.
//		It runs each assembler proc with pseudo-random values.
//		It validates ( asserts ) expected and returned results.
//		It also runs each repeatedly for comparative timings.
//		It provides a means to invoke and debug.
//		It illustrates calling the routines from C++.

#include "CppUnitTest.h"
#include "ui512_externs.h"
#include "ui512_unit_tests.h"
#include <cstring>
#include <format>
#include <sstream>
#include <string>
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ui512_Unit_Tests
{
	TEST_CLASS( ui512_unit_tests_division )
	{
		TEST_METHOD( ui512_01_div_edges )
		{
			u64 seed = 0;
			_UI512( num1 ) { 0 };
			_UI512( num2 ) { 0 };
			_UI512( dividend ) { 0 };
			_UI512( divisor ) { 0 };
			_UI512( expectedquotient ) { 0 };
			_UI512( expectedremainder ) { 0 };
			_UI512( quotient ) { 0 };
			_UI512( remainder ) { 0 };

			// Edge case tests

			// 1. zero divided by random
			zero_u( dividend );
			for ( int i = 0; i < test_run_count; i++ )
			{
				zero_u( dividend );
				RandomFill( divisor, &seed );
				zero_u( expectedquotient );
				zero_u( expectedremainder );
				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed zero divided by random on run #" << i ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed zero divided by random on run #" << i ) );
				};
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed zero divided by random" );
			};

			// 2.random divided by zero
			zero_u( dividend );
			for ( int i = 0; i < test_run_count; i++ )
			{
				RandomFill( dividend, &seed );
				zero_u( divisor );
				zero_u( expectedquotient );
				zero_u( expectedremainder );
				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed random divided by zero on run #" << i ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed random divided by zero on run #" << i ) );
				};
				Assert::AreEqual( s16( -1 ), retcode, L"Return code failed random divided by zero" );
			};

			// 3. random divided by one
			for ( int i = 0; i < test_run_count; i++ )
			{
				RandomFill( dividend, &seed );
				set_uT64( divisor, 1 );
				copy_u( expectedquotient, dividend );
				zero_u( expectedremainder );
				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed random divided by one on run #" << i ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed random divided by one on run #" << i ) );
				};
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed random divided by one" );
			};

			// 4. one divided by random
			for ( int i = 0; i < test_run_count; i++ )
			{
				zero_u( dividend );
				set_uT64( dividend, 1 );
				RandomFill( divisor, &seed );
				zero_u( expectedquotient );
				copy_u( expectedremainder, dividend );
				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed one divided by random on run #" << i ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed one divided by random on run #" << i ) );
				};
			};

			// 5. random divided by single word divisor, random bit 0->63
			// expected quotient is a shift right, expected remainder is a shift left
			for ( int i = 0; i < test_run_count; i++ )
			{
				RandomFill( dividend, &seed );
				u16 bitno = RandomU64( &seed ) % 63; // bit 0 to 63
				u64 divby = 1ull << bitno;
				set_uT64( divisor, divby );
				shr_u( expectedquotient, dividend, bitno );
				shl_u( expectedremainder, dividend, 512 - bitno );
				shr_u( expectedremainder, expectedremainder, 512 - bitno );
				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed random divided by one word of random bit during run #" << i ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed random divided by one word of random bit during run #" << i ) );
				};
			};

			{
				string test_message = _MSGA( "Divide function testing.\n Edge cases:\n\tzero divided by random,\n\trandom divided by zero,\n\trandom divided by one"
					"\n\tone divided by random,\n\trandom divided by one word of random bit.\n"
					<< test_run_count << " times each, with pseudo random values. \n" );
				Logger::WriteMessage( test_message.c_str( ) );
				Logger::WriteMessage( L"Passed. Non-volatile registers verified. Return code verified. Quotient and remainder verified; each via assert.\n\n" );
			};
		};
		TEST_METHOD( ui512_02_div_selected_dimensions )
		{
			u64 seed = 0;
			_UI512( num1 ) { 0 };
			_UI512( num2 ) { 0 };
			_UI512( num3 ) { 0 };
			_UI512( dividend ) { 0 };
			_UI512( divisor ) { 0 };
			_UI512( expectedquotient ) { 0 };
			_UI512( expectedremainder ) { 0 };
			_UI512( product ) { 0 };
			_UI512( overflow ) { 0 };
			_UI512( quotient ) { 0 };
			_UI512( remainder ) { 0 };

			for ( int i = 0; i < test_run_count; i++ )
			{
				num1 [ 0 ] = 0;
				num1 [ 1 ] = 0;
				num1 [ 2 ] = 0;
				num1 [ 3 ] = RandomU64( &seed );// &0x0000FFFFFFFFFFFF;
				num1 [ 4 ] = RandomU64( &seed );
				num1 [ 5 ] = RandomU64( &seed );
				num1 [ 6 ] = RandomU64( &seed );
				num1 [ 7 ] = RandomU64( &seed );

				num2 [ 0 ] = 0;
				num2 [ 1 ] = 0;
				num2 [ 2 ] = 0;
				num2 [ 3 ] = 0;
				num2 [ 4 ] = 0;
				num2 [ 5 ] = RandomU64( &seed );// | 0x0f00000000000000;
				num2 [ 6 ] = RandomU64( &seed );
				num2 [ 7 ] = RandomU64( &seed );

				num3 [ 7 ] = RandomU64( &seed );

				s16 pret = mult_u( product, overflow, num1, num2 );
				// set up 8 qword dividend = num1 * num2, divided by 3 qword divisor = num2
				copy_u( dividend, product );
				copy_u( divisor, num2 );
				copy_u( expectedquotient, num1 );
				zero_u( expectedremainder );

				s16 retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed specific value test" );

				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed specific value test" ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed specific value test" ) );
				};

				// set up 8 qword dividend = num1 * num2, divided by 5 qword divisor = num1
				copy_u( dividend, product );
				copy_u( divisor, num1 );
				copy_u( expectedquotient, num2 );
				zero_u( expectedremainder );

				retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed specific value test" );

				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed specific value test" ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed specific value test" ) );
				}

				// set up 8 qword dividend = num1 * num2 + num3, divided by 5 qword divisor = num1, add in what will be the remainder
				add_u( dividend, product, num3 );
				copy_u( divisor, num1 );
				copy_u( expectedquotient, num2 );
				copy_u( expectedremainder, num3 );

				retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed specific value test" );

				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed specific value test" ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed specific value test" ) );
				}


				// set up 8 qword dividend = num1 * num2 + num3, divided by 6 qword divisor = num1, add in what will be the remainder
				num1 [ 2 ] = RandomU64( &seed ); // make divisor a full 6 qword random
				num2 [ 5 ] = 0;
				mult_u( product, overflow, num1, num2 );
				add_u( dividend, product, num3 );
				copy_u( divisor, num1 );
				copy_u( expectedquotient, num2 );
				copy_u( expectedremainder, num3 );

				retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed specific value test" );

				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed specific value test" ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed specific value test" ) );
				}

				// set up 8 qword dividend = num1 * num2 + num3, divided by 2 qword divisor = num1, add in what will be the remainder

				mult_u( product, overflow, num1, num2 );
				add_u( dividend, product, num3 );
				copy_u( divisor, num2 );
				copy_u( expectedquotient, num1 );
				copy_u( expectedremainder, num3 );

				retcode = div_u( quotient, remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed specific value test" );

				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ], _MSGW( L"Quotient at word #" << j << " failed specific value test" ) );
					Assert::AreEqual( expectedremainder [ j ], remainder [ j ], _MSGW( L"Remainder at word #" << j << " failed specific value test" ) );
				}

				string test_message = _MSGA( "Divide function testing.\nDivide 8 by 3 random value qwords " << test_run_count << " times. Even divides, no remainders.\n" );
				test_message += _MSGA( "Divide 8 by 5 random value qwords " << test_run_count << " times. Even divides, no remainders.\n" );
				test_message += _MSGA( "Divide 8 by 5 random value qwords " << test_run_count << " times. Remainders expected and checked in actual.\n" );
				test_message += _MSGA( "Divide 8 by 6 random value qwords " << test_run_count << " times. Remainders expected and checked in actual.\n" );
				test_message += _MSGA( "Divide 8 by 2 random value qwords " << test_run_count << " times. Remainders expected and checked in actual.\n" );
				Logger::WriteMessage( test_message.c_str( ) );
				Logger::WriteMessage( L"Passed. Non-volatile registers verified. Return code verified. Quotients and remainders verified; each via assert.\n\n" );
			};
		};

		TEST_METHOD( ui512_03_div_index_limits )
		{
			u64 seed = 0;
			_UI512( num1 ) { 0 };
			_UI512( num2 ) { 0 };
			_UI512( num3 ) { 0 };
			_UI512( dividend ) { 0 };
			_UI512( divisor ) { 0 };
			_UI512( expectedquotient ) { 0 };
			_UI512( expectedremainder ) { 0 };
			_UI512( product ) { 0 };
			_UI512( overflow ) { 0 };
			_UI512( quotient ) { 0 };
			_UI512( remainder ) { 0 };
			int div_lt_div [ 8 ][ 8 ]; // table of dividend less than divisor cases, for expected quotient and remainder
			int div_eq_expected [ 8 ][ 8 ]; // table of dividend equal to divisor cases, for expected quotient and remainder
			int mult_overflow = 0;; // multiplication overflowed, so division was not attempted, and thus not tested, as that is not the focus of this test
			int div_lt_div_total = 0;
			int div_eq_expected_total = 0;
			int alltest_total = 0;

			for ( int trcnt = test_run_count; trcnt > 0; trcnt-- )
			{
				for ( int m = 0; m < 8; m++ )
				{
					for ( int n = 0; n < 8; n++ )
					{
						div_lt_div [ m ][ n ] = 0;
						div_eq_expected [ m ][ n ] = 0;
					}
				}
				for ( int m = 7; m >= 1; m-- )
				{
					RandomFill( num1, &seed );
					shr_u( num1, num1, ( 8 - m ) * 64 );
					for ( int n = 7; n >= 1; n-- )
					{
						alltest_total++;
						if ( m == 7 && n == 6 )
						{
							int stop = 1; // for debugging
						}
						RandomFill( num2, &seed );
						shr_u( num2, num2, ( n ) * 64 );
						mult_u( product, overflow, num1, num2 );
						s16 msb = msb_u( overflow );
						if ( msb != ( s16 ) -1 )
						{
							mult_overflow++;
							continue; // skip cases where product overflowed, as that is not the focus of this test
						}
						RandomFill( num3, &seed );
						shr_u( num3, num3, ( n + 1 ) * 64 ); // make num3 one qword smaller than divisor, so it can be the remainder when added to the product
						copy_u( dividend, product );
						s16 carry = add_u( dividend, dividend, num3 ); // add in what will be the remainder
						if ( carry != ( s16 ) 0 )
						{
							mult_overflow++;
							continue; // skip cases where product overflowed, as that is not the focus of this test
						}

						copy_u( divisor, num2 );
						copy_u( expectedquotient, num1 );
						copy_u( expectedremainder, num3 );
						s16 retcode = div_u( quotient, remainder, dividend, divisor );
						if ( compare_u( dividend, divisor ) == -1 ) // dividend less than divisor, expect quotient zero, remainder = dividend, return code 0
						{
							div_lt_div [ m ][ n ]++;
							div_lt_div_total++;
							Assert::AreEqual( s16( 0 ), retcode, L"Return code failed dividend less than divisor test" );
							for ( int k = 0; k < 8; k++ )
							{
								Assert::AreEqual( 0ull, quotient [ k ], _MSGW( L"Quotient at word #" << k << " failed dividend less than divisor test on run #" << trcnt << " for m=" << m << " n=" << n ) );
								Assert::AreEqual( dividend [ k ], remainder [ k ], _MSGW( L"Remainder at word #" << k << " failed dividend less than divisor test on run #" << trcnt << " for m=" << m << " n=" << n ) );
							}
						}
						else
						{
							div_eq_expected [ m ][ n ]++;
							div_eq_expected_total++;
							Assert::AreEqual( s16( 0 ), retcode, _MSGW( L"Return code failed specific value test on run #" << trcnt << " for m=" << m << " n=" << n ) );
							for ( int k = 0; k < 8; k++ )
							{
								Assert::AreEqual( expectedquotient [ k ], quotient [ k ], _MSGW( L"Quotient at word #" << k << " failed specific value test on run #" << trcnt << " for m=" << m << "  n=" << n ) );
								Assert::AreEqual( expectedremainder [ k ], remainder [ k ], _MSGW( L"Remainder at word #" << k << " failed specific value test on run #" << trcnt << " for m=" << m << " n=" << n ) );
							}
						}
					}
				}
			}
			string test_message = "Divide function testing.\nIndex limits:\nDividend less than divisor, expect quotient zero,";
			test_message += " remainder = dividend, return code 0.\nDividend equal to or greater than divisor, ";
			test_message += " expect quotient and remainder as calculated, return code 0.\n";
			test_message += format( "{:04d}", test_run_count );
			test_message += " runs with random values, ";
			test_message += " covering all index positions for dividend and divisor from 1 to 8 qwords.\n";
			test_message += format( "Multiplication overflowed, so division was not attempted, and thus not tested, in {:d} cases. {:8.2f}%\n", mult_overflow, double( ( double ) mult_overflow / ( double ) alltest_total * 100.0 ) );
			test_message += format( "Dividend less than divisor cases: {:d} total, {:8.2f}%\n", div_lt_div_total, double( ( double ) div_lt_div_total / ( double ) alltest_total * 100.0 ) );
			test_message += format( "Quotient and Remainder equal expected (divide parameters ok): {:d} total, with {:8.2f}%\n", div_eq_expected_total, double( ( double ) div_eq_expected_total / ( double ) alltest_total * 100.0 ) );

			test_message += "Passed. Non-volatile registers verified. Return code verified. Quotients and remainders verified; each via assert.\n\n";
			Logger::WriteMessage( test_message.c_str( ) );

		}

		TEST_METHOD( ui512_04_div_performance )
		{
			// Performance timing tests.
			// Ref: "Essentials of Modern Business Statistics", 7th Ed, by Anderson, Sweeney, Williams, Camm, Cochran. South-Western, 2015
			// Sections 3.2, 3.3, 3.4
			// Note: these tests are not pass/fail, they are informational only

			Logger::WriteMessage( L"Divide function performance timing test.\n\n" );

			Logger::WriteMessage( L"First run.\n" );
			perf_stats No1 = Perf_Test_Parms [ 0 ];
			ui512_Unit_Tests::RunStats( &No1, Div );

			Logger::WriteMessage( L"Second run.\n" );
			perf_stats No2 = Perf_Test_Parms [ 1 ];
			RunStats( &No2, Div );

			Logger::WriteMessage( L"Third run.\n" );
			perf_stats No3 = Perf_Test_Parms [ 2 ];
			RunStats( &No3, Div );
		};

		TEST_METHOD( ui512_10_div64 )
		{
			u64 seed = 0;
			_UI512( dividend ) { 0 };
			_UI512( quotient ) { 0 };
			_UI512( expectedquotient ) { 0 };

			u64 divisor = 0;
			u64 remainder = 0;
			u64 expectedremainder = 0;

			// Edge case tests
			// 1. zero divided by random
			zero_u( dividend );
			for ( int i = 0; i < test_run_count; i++ )
			{
				zero_u( dividend );
				divisor = RandomU64( &seed );
				zero_u( expectedquotient );
				expectedremainder = 0;
				s16 retcode = div_uT64( quotient, &remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ],
						_MSGW( L"Quotient at word #" << j << " failed zero divided by random on run #" << i ) );
				};
				Assert::AreEqual( expectedremainder, remainder,
					_MSGW( L"Remainder failed zero divided by random on run #" << i ) );
			};
			// 2. random divided by one
			for ( int i = 0; i < test_run_count; i++ )
			{
				RandomFill( dividend, &seed );
				divisor = 1;
				copy_u( expectedquotient, dividend );
				expectedremainder = 0;
				s16 retcode = div_uT64( quotient, &remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ],
						_MSGW( L"Quotient at word #" << j << " failed random divided by one on run #" << i ) );
				};
				Assert::AreEqual( expectedremainder, remainder,
					_MSGW( L"Remainder failed random divided by one " << i ) );
			};
			// 3. random divided by self
			for ( int i = 0; i < test_run_count; i++ )
			{
				zero_u( dividend );
				dividend [ 7 ] = RandomU64( &seed );
				divisor = dividend [ 7 ];
				set_uT64( expectedquotient, 1 );
				expectedremainder = 0;
				s16 retcode = div_uT64( quotient, &remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ],
						_MSGW( L"Quotient at word #" << j << " failed random divided by self on run #" << i ) );
				};
				Assert::AreEqual( expectedremainder, remainder,
					_MSGW( L"Remainder failed random divided by self " << i ) );
			};
			{
				string test_message = _MSGA( "Divide (u64) function testing.\n\n Edge cases:\n\tzero divided by random,\n\trandom divided by one,\n\trandom divided by self.\n "
					<< test_run_count << " times each, with pseudo random values.\n";);
				Logger::WriteMessage( test_message.c_str( ) );
				Logger::WriteMessage( L"Passed. Non-volatile registers verified. Return code verified. Quotient and remainder verified; each via assert.\n\n" );
			};

			// First test, a simple divide by two. 
			// Easy to check as the expected answer is a shift right,
			// and expected remainder is a shift left

			for ( int i = 0; i < test_run_count; i++ )
			{
				RandomFill( dividend, &seed );
				zero_u( quotient );
				divisor = 2;
				shr_u( expectedquotient, dividend, u16( 1 ) );
				expectedremainder = ( dividend [ 7 ] << 63 ) >> 63;
				s16 retcode = div_uT64( quotient, &remainder, dividend, divisor );
				Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
				for ( int j = 0; j < 8; j++ )
				{
					Assert::AreEqual( expectedquotient [ j ], quotient [ j ],
						_MSGW( L"Quotient at word #" << j << " failed on run #" << i ) );
				};
				Assert::AreEqual( expectedremainder, remainder, _MSGW( L"Remainder failed " << i ) );
			};
			{
				string test_message = _MSGA( "Divide (u64) function testing. Simple divide by 2 "
					<< test_run_count << " times, each with pseudo random values.\n" );
				Logger::WriteMessage( test_message.c_str( ) );
				Logger::WriteMessage( L"Passed. Non-volatile registers verified. Return code verified. Quotient and remainder verified; each via assert.\n\n" );
			};

			// Second test, a simple divide by sequential powers of two. 
			// Still relatively easy to check as expected answer is a shift right,
			// and expected remainder is a shift left

			for ( u16 nrShift = 0; nrShift < 64; nrShift++ )	// rather than a random bit, cycle thru all 64 bits 
			{
				for ( int i = 0; i < test_run_count / 64; i++ )
				{
					RandomFill( dividend, &seed );
					divisor = 1ull << nrShift;
					shr_u( expectedquotient, dividend, nrShift );
					expectedremainder = ( nrShift == 0 ) ? 0 : ( dividend [ 7 ] << ( 64 - nrShift ) ) >> ( 64 - nrShift );
					s16 retcode = div_uT64( quotient, &remainder, dividend, divisor );
					Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
					for ( int j = 0; j < 8; j++ )
					{
						Assert::AreEqual( expectedquotient [ j ], quotient [ j ],
							_MSGW( L"Quotient at word #" << j << " failed shifting: " << nrShift << " on run #" << i ) );
					}
					Assert::AreEqual( expectedremainder, remainder, _MSGW( L"Remainder failed shifting: " << nrShift << " on run #" << i ) );
				};
			}
			{
				string test_message = _MSGA( "Divide function testing. Divide by sequential powers of 2 "
					<< test_run_count << " times, each with pseudo random values.\n" );
				Logger::WriteMessage( test_message.c_str( ) );
				Logger::WriteMessage( L"Passed. Non-volatile registers verified. Return code verified. Quotient and remainder verified; each via assert.\n\n" );
			}

			// Third test, Use case tests, divide out to get decimal digits. Do whole with random,
			// and a knowable sample 
			{
				string digits = "";
				RandomFill( dividend, &seed );
				int comp = compare_uT64( dividend, 0ull );
				int cnt = 0;
				while ( comp != 0 )
				{
					s16 retcode = div_uT64( dividend, &remainder, dividend, 10ull );
					Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
					char digit = 0x30 + char( remainder );
					digits.insert( digits.begin( ), digit );
					comp = compare_uT64( dividend, 0ull );
					if ( comp != 0 )
					{
						cnt++;
						if ( cnt % 30 == 0 )
						{
							digits.insert( digits.begin( ), '\n' );
						}
						else
						{
							if ( cnt % 3 == 0 )
							{
								digits.insert( digits.begin( ), ',' );
							};
						};
					}
				}
				Logger::WriteMessage( L"Use case: Divide to extract decimal digits:\n" );
				Logger::WriteMessage( digits.c_str( ) );
				Logger::WriteMessage( L"\nPassed. Non-volatile registers verified. Return code verified; each via assert.\n" );
			}
			{
				string digits = "";
				u64 num = 12345678910111213ull;
				set_uT64( dividend, num );
				int comp = compare_uT64( dividend, 0ull );
				int cnt = 0;
				while ( comp != 0 )
				{
					s16 retcode = div_uT64( dividend, &remainder, dividend, 10ull );
					Assert::AreEqual( s16( 0 ), retcode, L"Return code failed one divided by random" );
					char digit = 0x30 + char( remainder );
					digits.insert( digits.begin( ), digit );
					comp = compare_uT64( dividend, 0ull );
					if ( comp != 0 )
					{
						cnt++;
						if ( cnt % 3 == 0 )
						{
							digits.insert( digits.begin( ), ',' );
						};
					}

				}
				string expected = "12,345,678,910,111,213";
				Assert::AreEqual( expected, digits );
				Logger::WriteMessage( L"\nUse case: Divide to extract known decimal digits:\n" );
				Logger::WriteMessage( digits.c_str( ) );
				Logger::WriteMessage( L"\nPassed.\n\tNon-volatile registers verified.\n\tReturn code verified.\n\tExtracted decimal digits verified.\nEach via assert.\n\n" );
			}
		}
		TEST_METHOD( ui512_11_div64_performance )
		{
			// Performance timing tests.
			// Ref: "Essentials of Modern Business Statistics", 7th Ed, by Anderson, Sweeney, Williams, Camm, Cochran. South-Western, 2015
			// Sections 3.2, 3.3, 3.4
			// Note: these tests are not pass/fail, they are informational only

			Logger::WriteMessage( L"Divide x64 function performance timing test.\n\n" );

			Logger::WriteMessage( L"First run.\n" );
			perf_stats No1 = Perf_Test_Parms [ 0 ];
			ui512_Unit_Tests::RunStats( &No1, Div64 );

			Logger::WriteMessage( L"Second run.\n" );
			perf_stats No2 = Perf_Test_Parms [ 1 ];
			RunStats( &No2, Div64 );

			Logger::WriteMessage( L"Third run.\n" );
			perf_stats No3 = Perf_Test_Parms [ 2 ];
			RunStats( &No3, Div64 );
		};
	};
};
