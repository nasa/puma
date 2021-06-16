#ifndef TppInput_H
#define TppInput_H

class TPMSInput{
public:
    int xSize;
    int ySize;
    int zSize;
    double wMax;
    double wMin;
    double qMax;
    double qMin;
    int equation;


    TPMSInput(int xSize, int ySize, int zSize, double wMin, double wMax, double qMin, double qMax, int equation=0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->wMin = wMin;
        this->wMax = wMax;
        this->qMin = qMin;
        this->qMax = qMax;
        this->equation = equation;

    }

    TPMSInput() {
        this->xSize = -1;
        this->ySize = -1;
        this->zSize = -1;
        this->wMin = -1;
        this->wMax = -1;
        this->qMin = -1;
        this->qMax = -1;
        this->equation = -1;
    }


};

#endif // TppInput_H
