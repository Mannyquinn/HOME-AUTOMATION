 MAX487::MAX487(int RE_PIN, int DE_PIN):RE_PIN(RE_PIN),DE_PIN(DE_PIN)
  {
    pinMode(RE_PIN, OUTPUT);
    pinMode(DE_PIN, OUTPUT);
  }
  void MAX487::enableTransmission()
  {
    digitalWrite(DE_PIN, HIGH);
    digitalWrite(RE_PIN, HIGH);
    
  }
  void MAX487::disableTransmissionEnableReception()
  {
    digitalWrite(DE_PIN, LOW);
    digitalWrite(RE_PIN, LOW);
  }
  
  void MAX487::shutdownDevice()
  {
    digitalWrite(DE_PIN, LOW);
    digitalWrite(RE_PIN, HIGH);
  }
