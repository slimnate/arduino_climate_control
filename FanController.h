class FanController {
    private:
        byte controlPin;
        bool enabled;
        
    public:
        FanController(byte pin);

        bool isEnabled();
        void enable();
        void disable();
};