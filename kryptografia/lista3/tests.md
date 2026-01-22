# Diffie-Hellman

## $F_p$
* $p=23$
* $g=5$
* $q=22$
* $sk=6$ ($pk=8$)
* Received $pk=19$
* Expected shared secret $=2$

## $F_{2^k}$
* Irreducible polynomial $=19$
* $g=2$
* $q=15$
* $sk=2$ ($pk=4$)
* Received $pk=8$
* Expected shared secret $=12$

## $EC(F_p)$
* $p=23$
* Curve $a=1$, $b=1$
* $G=(0, 1)$
* $q=28$
* $sk=2$ ($pk=(6, 19)$)
* Received $pk=(6, 19)$
* Expected shared secret $=(13, 16)$ (HEX: `0D;10`)

# Schnorr ($F_p$)

## Sign
* $p=23$ (HEX: `17`)
* $g=5$
* $q=22$
* $sk=3$ ($pk=10$, HEX: `0A`)
* Message $= \text{"test"}$
* **Action:** Wygeneruj podpis $(s, e)$

## Verify
* Signer $pk=10$ (HEX: `0A`)
* Signature $s, e$ (z poprzedniego kroku)
* Message $= \text{"test"}$
* Expected result $= \text{VALID}$

# Timing Analysis ($F_p$)

* $p=340282366920938463463374607431768211297$ (Duża liczba pierwsza)
* $g=2$
