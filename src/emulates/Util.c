int get_bit(int startbit, int length, int instruction) {
    int mask = 0;
    for (int i = 0; i < length; i++) {
        mask += 1;
        mask <<= 1;
    }
    return (instruction >> (startbit - length)) & mask;
}