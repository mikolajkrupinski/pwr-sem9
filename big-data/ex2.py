import mpmath as mp

seq = "310802"

mp.mp.dps = 5_000_000

pi_str = str(mp.pi)[2:]
pi_pos = pi_str.find(seq)
print("π:", pi_pos)
print(f"...{pi_str[pi_pos-1:pi_pos+len(seq)+2]}...")

sqrt2_str = str(mp.sqrt(2))[2:]
e_str = str(mp.e)[2:]
e_pos = e_str.find(seq)
print("e:", e_pos)
print(f"...{e_str[e_pos-1:e_pos+len(seq)+2]}...")

sqrt2_pos = sqrt2_str.find(seq)
print("√2:", sqrt2_pos)
print(f"...{sqrt2_str[sqrt2_pos-1:sqrt2_pos+len(seq)+2]}...")
