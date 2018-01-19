StopTimer.timeout.connect(
    function()
    {
        if(times == 2)
        {
            tmpDlg.on_Floor_FuncCase_Stop_Btn_clicked();
        }
    }
);
StopTimer.start(500);
