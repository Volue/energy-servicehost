$searchText='<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />'


$modifiedProjects = 0
$numberOfProjects = 0
Foreach ($file in (Get-Childitem ..\*.vcxproj -Recurse))
{
    Set-Location $file.DirectoryName
    $relativePath = Resolve-Path -Relative $PSScriptRoot
    $insertText='<Import Project="'+$relativePath+'\toolset.props" />'
    $numberOfProjects = $numberOfProjects + 1
    if ($file.Length)
    {
        'Processing '+$file.BaseName+' Project'
        (Get-Content $file.FullName).Replace($searchText,$insertText+"`n"+$searchText) | Set-Content $file.FullName
        
        $text = Get-Content $file.FullName
        ForEach ($line in $text)
        {
            if ($line.Contains($searchText))
            {
                $modifiedProjects = $modifiedProjects + 1
            }
        }
    }
}
Set-Location $PSScriptRoot
"Total Number of Projects: "+$numberOfProjects
$modifiedProjects.ToString()+" projects has been modified"