#!/usr/bin/perl
use strict;

sub main {
    my $dataDir = "../data/";
    my $heroinDir = "../heroin/";
    my $skillDataFile = $dataDir . "skills.txt";
    my $skillHppFile  = $heroinDir . "skills.hpp";
    parseSkill($skillDataFile, $skillHppFile);
}

sub parseSkill {
    my $inFile = shift;
    my $outFile = shift;
    if (! -f $inFile) {
        die("cannot find skill file at $inFile\n");
    }

    my @skillList;
    open FOO, "<" . $inFile;
    while (my $skill = <FOO>) {
        chomp($skill);
        $skill = lc($skill);
        $skill =~ s/[\s\-]/_/gs;
        push @skillList, $skill;
    }
    close FOO;

    my @enumList;
    for(my $i=0; $i < $#skillList; $i++) {
        my $comma = ($i == $#skillList-1 ? " " : ",");
        my $skill = $skillList[$i] . $comma;
        my $enum = "        " .
            sprintf("%-30s // %4d 0x%04x", $skill, $i, $i);
        push @enumList, $enum;
    }

    my $out = join("\n", @enumList);
    my $template = getSkillTemplate();
    $template =~ s/\[\%list\%\]/$out/;

    print "writing skills file to $outFile\n";
    writeToFile($template, $outFile);
}

sub getSkillTemplate {
    my $template = <<END;
// Do not modify
// Generated from $0
// list stolen from D2BS SDK, run through a Python script to generate enums
#ifndef SKILLS_HPP
#define SKILLS_HPP
namespace skill
{
    enum skill_type 
    {
[%list%]
    };
}

using skill::skill_type;
#endif
END
    return $template;
}

sub writeToFile {
    my $str = shift;
    my $outFile = shift;
    open FOO, ">" . $outFile;
    print FOO $str;
    close FOO;
}

main();
