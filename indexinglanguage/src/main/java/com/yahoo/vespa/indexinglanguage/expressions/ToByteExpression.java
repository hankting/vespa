// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.vespa.indexinglanguage.expressions;

import com.yahoo.document.DataType;
import com.yahoo.document.DocumentType;
import com.yahoo.document.datatypes.ByteFieldValue;

/**
 * @author Simon Thoresen Hult
 */
public class ToByteExpression extends Expression {

    @Override
    protected void doExecute(ExecutionContext ctx) {
        ctx.setValue(new ByteFieldValue(Byte.valueOf(String.valueOf(ctx.getValue()))));
    }

    @Override
    protected void doVerify(VerificationContext context) {
        context.setValue(createdOutputType());
    }

    @Override
    public DataType requiredInputType() {
        return UnresolvedDataType.INSTANCE;
    }

    @Override
    public DataType createdOutputType() {
        return DataType.BYTE;
    }

    @Override
    public String toString() {
        return "to_byte";
    }

    @Override
    public boolean equals(Object obj) {
        return obj instanceof ToByteExpression;
    }

    @Override
    public int hashCode() {
        return getClass().hashCode();
    }
}
