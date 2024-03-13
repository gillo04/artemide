enum {
    S_IDLE,
    S_TRACING
};

enum {
    C_SELECT,
    C_WIRE,
    C_RESISTOR,
    C_NODE
};

typedef struct {
    int type;
    int a, b;
    float value;
} Node;

