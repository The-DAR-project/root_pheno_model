double calculateTLoss(double T0, double t, double meanT) {
    return T0 * TMath::Exp(-t / meanT);
}

void thermo() {
    double T0 = 40;    // Example initial value
    double t = 10;      // Example time
    double meanT = 340;  // Example mean time

    double TLoss = calculateTLoss(T0, t, meanT);
    std::cout << "T_loss = " << TLoss << std::endl;
}