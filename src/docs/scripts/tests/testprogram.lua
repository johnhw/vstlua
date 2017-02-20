help.title="Tests program persistence"
help.description="Tests program persistence."
help.author="tzec"

--callback functionality
g1 = addControl{x=50, y=5, type=GUITypes.text, label="", callback=function (value,text) programData.testStoredValue=text  end}


function loadProgramCb()
    if programData.testStoredValue then
        vprint(programData.testStoredValue)
    end
end