#!/usr/bin/perl
use strict;

sub main {
    my $dataDir = "../data/";
    my $heroinDir = "../heroin/";
    my $skillDataFile = $dataDir . "skills.txt";
    my $skillHppFile  = $heroinDir . "skills.hpp";
    my $skillCppFile  = $heroinDir . "skills.cpp";
    parseSkill($skillDataFile, $skillHppFile, $skillCppFile);
}

sub parseSkill {
    my $inFile  = shift;
    my $hppFile = shift;
    my $cppFile = shift;
    if (! -f $inFile) {
        die("cannot find skill file at $inFile\n");
    }

    my @skillList;
    foreach my $skill (split /\n/, readFromFile($inFile)) {
        $skill = lc($skill);
        $skill =~ s/[\s\-]/_/gs;
        push @skillList, $skill;
    }

    my @enumList;
    for(my $i=0; $i < $#skillList; $i++) {
        my $comma = ($i == $#skillList-1 ? " " : ",");
        my $skill = $skillList[$i] . $comma;
        my $enum = "        " .
            sprintf("%-30s // %4d 0x%04x", $skill, $i, $i);
        push @enumList, $enum;
    }

    my $enumList = join("\n", @enumList);
    my $template = getSkillsHppTemplate();
    $template =~ s/\[\%enumList\%\]/$enumList/;

    print "writing skills hpp file $hppFile\n";
    writeToFile($template, $hppFile);

    my @switchList;
    for(my $i=0; $i < $#skillList; $i++) {
        my $skillWithColon = $skillList[$i] . ":";
        push @switchList, 
            "    case " . 
            sprintf("%-30s // %4d 0x%04x\n", $skillWithColon, $i, $i) .
            "        return \"" . $skillList[$i] . "\";\n" .
            "        break;\n";
    }
    my $switchList = join("", @switchList);
    my $cppTemplate = getSkillsCppTemplate();
    $cppTemplate =~ s/\[\%switchList\%\]/$switchList/;

    print "writing skills cpp file $cppFile\n";
    writeToFile($cppTemplate, $cppFile);

}


sub readFromFile {
    my $inFile = shift;
    local $/=undef;
    open FILE, $inFile or die "Couldn't open $inFile: $!";
    binmode FILE;
    my $string = <FILE>;
    close FILE;
    return $string;
}

sub writeToFile {
    my $str = shift;
    my $outFile = shift;
    open FOO, ">" . $outFile;
    print FOO $str;
    close FOO;
}

sub getSkillsHppTemplate {
    return <<END;
// Do not modify. Generated from $0
// list stolen from D2BS SDK, run through a Python script to generate enums
#ifndef SKILLS_HPP
#define SKILLS_HPP
#include <string>
namespace skill
{
    enum skill_type 
    {
[%enumList%]
    };
    std::string skill_to_string(skill_type type);
}
#endif
END
}

sub getSkillsCppTemplate {
    return <<END;
// Do not modify. Generated from $0
#include <heroin/skills.hpp>
#include <string>
std::string skill::skill_to_string(skill_type type) {
    switch(type) {
[%switchList%]
    default:
        return "unknown";
        break;
    }
}
END
}

main();
