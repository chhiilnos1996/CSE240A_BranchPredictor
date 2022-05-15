# k number of last bits of pc
# m number of bits for local branch pattern
# l number of bits for global history

# local pht 2^k*m
# local branch t/nt 2 bit sc 2^m*2
# global branch t/nt 2 bit sc 2^l*2
# choice predictor 2^l*2
# 32 Kbits

for l in range(10, 14):
    for k in range(5, 14):
        for m in range(5, 14):
            result = pow(2,l+2)+pow(2,m+1)+m*pow(2,k)
            if result<=pow(2,15):
              print(m,k,l)

#k'<k or m'<m or l'<l

# files predictor.c predictor.h main.c
# 12 ghr 3.00 on gradescope 4.64 docker
# tournament to beat 2.6, 2.7
 
# 13 9 11 -> 2.07
# 11 11 10 -> 2.23
# 12 10 11 -> 2.07
# 10 11 11 -> 2.41
# 11 10 12 -> 2.07
# 12 9 12 -> 2.21
# 10 10 12 -> 2.34
