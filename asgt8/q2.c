int q2(int a, int b, int c) {
    int jumptable[9] = {816, 900, 820, 900, 828, 852, 900, 876};

    if (a < 10 || a > 18) {
        c = 0;
        return c;
    } else {
        switch(jumptable[a - 10]) {
            case 816:
            c = c + b;
            break;

            case 900:
            c = 0;
            break;

            case 820:
            c = b - c;
            break;

            case 828:
            if (b > c) {
                c = 1;
            } else {
                c = 0;
            }
            break;
            
            case 852:
            if (c > b) {
                c = 1;
            } else {
                c = 0;
            }
            break;

            case 876:
            if (b == c) {
                c = 1;
            } else {
                c = 0;
            }
            break;
        }
        return c;
    }
}