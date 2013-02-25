__attribute__((annotate("kernel")))
void f(int *in, int *out, int M, int N) {

    int i, j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            out[i + j] = in[i] + in[j];
        }
    }

}
