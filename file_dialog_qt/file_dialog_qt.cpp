#include <QApplication>
#include <QFileDialog>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << "\n";
        return 0;
    }

    QString mode = argv[1];
    QString caption;
    
    if(argc >= 3)
    {
        caption = argv[2];
    }
    
    int null = 0;
    QApplication app(null, argv);

    QString text;
    
    if(mode == "open")
    {
        if(caption.isEmpty())
            caption = "Open File";
        text = QFileDialog::getOpenFileName(0, caption);
    }
    else if(mode == "save")
    {
        if(caption.isEmpty())
            caption = "Save File";
        text = QFileDialog::getSaveFileName(0, caption);
    }
    else if(mode == "dir")
    {
        if(caption.isEmpty())
            caption = "Directory";
        text = QFileDialog::getExistingDirectory(0, caption);
    }
    else
    {
        text = "error";
    }
    
    cout << text.toStdString() << "\n";
    
    return 0;
}
