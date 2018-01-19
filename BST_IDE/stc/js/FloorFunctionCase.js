var FloorCount = 1;
times = 1;
ChangeFloorTimer.timeout.connect(
    function()
    {
        if(FloorCount <42)
        {
            FloorCount++;
            m_stcTest.IBHostParserTest.SetFloor(FloorCount);
                switch(times)
                {
                    case 1:
                        m_stcTest.IBHostParserTest.SetVoiceEnable(true);
                        m_stcTest.IBHostParserTest.SetStationArrive(false);
                        break;
                    case 2:
                        m_stcTest.IBHostParserTest.SetVoiceEnable(false);
                        m_stcTest.IBHostParserTest.SetStationArrive(false);
                        break;
                    case 3:
                        m_stcTest.IBHostParserTest.SetVoiceEnable(false);
                        m_stcTest.IBHostParserTest.SetStationArrive(true);
                        break;
                    case 4:
                        m_stcTest.IBHostParserTest.SetVoiceEnable(false);
                        m_stcTest.IBHostParserTest.SetStationArrive(false);
                        break;
                    case 5:
                        m_stcTest.IBHostParserTest.SetVoiceEnable(true);
                        m_stcTest.IBHostParserTest.SetStationArrive(true);
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
            m_stcTest.IBHostParserTest.SetDisablityMode(true);
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
            m_stcTest.ChangeFloorTimer.disconnect(0,0,0);
            m_stcTest.ChangeFloorTimer.stop();
            m_stcTest.SendInstructionTimer.disconnect(0,0,0);
            m_stcTest.SendInstructionTimer.stop();
            m_stcTest.StopTimer.disconnect(0,0,0);
            m_stcTest.StopTimer.stop();
        }
    }
);
StopTimer.start(500);
