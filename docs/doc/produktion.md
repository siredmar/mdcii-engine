# Production sequence

Notes on changes in the Prodlist data between saved scores immediately after each other

    0e 05 29

active building without resources: wait step 3 probably reduced by 4 to 10 => ani increased from 3 to 4 and load_denominator from 179 to 190

    0e 28 10

load_denominator = 231 => load_denominator = 121 (= (231+11)/2 )
    0e 0b 18
229 => 120

Assumption for ani: Number of zero crossings of the waiting step without production

With production

    0e 0c 25
    0e 13 16
    0e 13 1e

Assumed sequence

wait step is reduced by 1 every second.
At zero crossing

-   a modulo dependent on the building type is added to wait step.
-   the modulo is added to occupancy_denominator
-   if the building is currently producing (i.e. if there are enough raw materials)
    -   another type-dependent value added to load_counter
    -   ani set to 0
    -   a type-dependent value of raw material (and raw material2) deducted
    -   a type-dependent value added to product
-   otherwise
    -   ani increased by 1 (not above 0x0f)

If utilization_denominator exceeds a certain value, utilization_denominator and utilization_counter are halved.

Translated with www.DeepL.com/Translator (free version)
