class AtomizerController {
    private:
        byte controlPin;
        bool enabled;

    public:
        AtomizerController(byte pin);

        bool isEnabled();
        void enable();
        void disable();
};