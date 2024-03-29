## Code - TODO: Comment and translate to C in q1()
.pos 0x1000

## C statement 1
S1:
ld    $i, r0            # r0 = addr. of i
ld    (r0), r0          # r0 = i
ld    $a, r1            # r1 = addr. of a
ld    (r1), r1          # r1 = a (= addr. of a->x)
ld    (r1), r1          # r1 = a->x (= addr. of int)
ld    (r1, r0, 4), r2   # r2 = deref. of a->x[i]
ld    $v0, r3           # r3 = addr. of v0
st    r2, (r3)          # v0 = a->x[i]

## C statement 2
S2:
ld    $i, r0            # r0 = addr. of i
ld    (r0), r0          # r0 = i
ld    $a, r1            # r1 = addr. of a
ld    (r1), r1          # r1 = a (= addr. of a->x)
inca  r1                # r1 = a + 4 (= addr. of a->b.y[0])
ld    (r1, r0, 4), r2   # r2 = a->b.y[i]
ld    $v1, r3           # r3 = addr. of v1
st    r2, (r3)          # v1 = a->b.y[i]

## C statement 3
S3:
ld    $i, r0            # r0 = addr. of i
ld    (r0), r0          # r0 = i
ld    $a, r1            # r1 = addr. of a
ld    (r1), r1          # r1 = a (= addr. of a->x)
ld    20(r1), r1        # r1 = a->b.a (= addr. of struct A)
ld    (r1), r1          # r1 = a->b.a->x (= addr. of int)
ld    (r1, r0, 4), r2   # r2 = deref. of a->b.a->x[i]
ld    $v2, r3           # r3 = addr. of v2
st    r2, (r3)          # v2 = a->b.a->x[i]

## C statement 4
S4:
ld    $a, r1            # r1 = addr. of a
ld    (r1), r1          # r1 = a (addr. of a->x)
st    r1, 20(r1)        # a->b.a = a (addr. of a->x)

## C statement 5
S5:
ld    $i, r0            # r0 = addr. of i
ld    (r0), r0          # r0 = i
ld    $a, r1            # r1 = addr. of a
ld    (r1), r1          # r1 = a (= addr. of a->x)
ld    20(r1), r1        # r1 = a->b.a (= addr. of a->b.a->x)
inca  r1                # r1 = r1 + 4 (= addr. of a->b.a->b.y[0])
ld    (r1, r0, 4), r2   # r2 = a->b.a->b.y[i]
ld    $v3, r3           # r3 = addr. of v3
st    r2, (r3)          # v3 = a->b.a->b.y[i]


halt


## Globals
.pos 0x2000
i:  .long 1
v0: .long 0
v1: .long 0
v2: .long 0
v3: .long 0
a:  .long d0


## Heap (these labels represent dynamic values and are thus not available to code)
.pos 0x3000
d0: .long d1
    .long 20
    .long 21
    .long 22
    .long 23
    .long d2
d2: .long d3
    .long 40
    .long 41
    .long 42
    .long 43
    .long 0
d1: .long 10
    .long 11
    .long 12
    .long 13
d3: .long 30
    .long 31
    .long 32
    .long 33
