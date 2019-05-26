/**
 * Check for division by zero
 */
unsigned int __aeabi_uidiv (unsigned int numerator, unsigned int denominator)
{
        unsigned int result;
        result = 0;
        while ((numerator-denominator)>denominator)
        {
                result++;
                numerator -=denominator;
        }
        return result;
}
