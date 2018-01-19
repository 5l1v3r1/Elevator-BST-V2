var FloorCount = 1;
times = 1;
ChangeFloorTimer.timeout.connect(
    function()
    {
        if(FloorCount <42)
        {
            FloorCount++;
            IBHostParserTest.SetFloor(FloorCount);
                switch(times)
                {
                    case 1:
                        IBHostParserTest.SetVoiceEnable(true);
                        IBHostParserTest.SetStationArrive(false);
                        break;
                    case 2:
                        IBHostParserTest.SetVoiceEnable(false);
                        IBHostParserTest.SetStationArrive(false);
                        break;
                    case 3:
                        IBHostParserTest.SetVoiceEnable(false);
                        IBHostParserTest.SetStationArrive(true);
                        break;
                    case 4:
                        IBHostParserTest.SetVoiceEnable(false);
                        IBHostParserTest.SetStationArrive(false);
                        break;
                    case 5:
                        IBHostParserTest.SetVoiceEnable(true);
                        IBHostParserTest.SetStationArrive(true);
                        break;
                }
        }
        else
        {
            FloorCount++;
            times++;
        }
    }
);
ChangeFloorTimer.start(500);

SendInstructionTimer.timeout.connect(
    function()
    {
        if(FloorCount <= 42)
        {
            IBHostParserTest.SetDisablityMode(true);
        }
    }
);
SendInstructionTimer.start(50);

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
