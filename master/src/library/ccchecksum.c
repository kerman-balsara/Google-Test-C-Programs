// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#include <ccchecksum.h>

// --------------------------------------------------------------------
// Check whether passed credit card number has a valid check sum
// (LUHN 16).
//
// Return:
// 1 - Valid
// 0 - Invalid
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/ccchecksum.cpp
// --------------------------------------------------------------------

// https://www.groundlabs.com/blog/anatomy-of-a-credit-card/
// Calculating the Luhn algorithm by hand includes a few different steps. They include the following.
// 1. Write down the credit card number:
// 4417 1234 5678 9113
// 2. Starting from the first number, double every other digit.
// 4(x2) 4 1(x2) 7 1(x2) 2 3(x2) 4 5(x2) 6 7(x2) 8 9(x2) 1 1(x2) 3
// The doubled numbers result in: 8 2 2 6 10 14 18 2
// 3. If the result of the doubling ends up with two digits, then add those two digits together:
// 10 = 1+0 14= 1+4 18= 1+8
// 4. Add up all numbers: 8+4+2+7 + 2+2+6+4 + 1+0+6+1+4+8 + 1+8+1+2+3 = 70
// If the final sum is divisible by 10, then the credit card is valid. If it is not divisible by 10,
// the number is invalid or fake.

bool_t IsValidCreditCardCheckSum(constr_t ccNo)
{
    int ccLen = strlen(ccNo);

    int sum = ccNo[ccLen - 1] - '0';
 
    int dbl = 1;

    for (int i = ccLen - 2; i >= 0; i--)
    {
        int digit = ccNo[i] - '0';
        int value = digit;

        if (dbl)
        {
            value *= 2;
            if (value > 9) value -= 9;
        }

        sum += value;
        dbl = !dbl;
    }

    return((sum % 10) ? 0 : 1);
}