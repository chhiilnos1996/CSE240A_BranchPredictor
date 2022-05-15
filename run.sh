bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom

bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom

bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom

bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom

bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom

bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom


# test only gshare
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare

# test only tournament
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament

# test only custom
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom
