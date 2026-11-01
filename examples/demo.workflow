# TIVIDY Example workflow
# Generic sample using simple task and role names.
#
# Commands:
#   workflow | <workflow name> | <version>
#   task     | <task name> | <required role>
#   approve  | <trigger> | <approval chain name>
#   stage    | <stage name> | sequential|parallel
#   end
#   done

workflow | TIVIDY Example | 1

# A root-level task. It must be completed and approved before Stage1 begins.
task | Task1 | role1
approve | complete | review

# Sequential stage: Task3 stays Pending until Task2 is completed.
stage | Stage1 | sequential
    task | Task2 | role1
    task | Task3 | role2
end

# Parallel stage: Task4 and Task7 can become Available together.
# SubStage1 is nested inside Stage2 and runs sequentially.
stage | Stage2 | parallel
    task | Task4 | role1

    stage | SubStage1 | sequential
        task | Task5 | role2
        task | Task6 | role1
    end

    task | Task7 | role2
end

done
